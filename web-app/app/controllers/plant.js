const express = require('express');
const router = express.Router();
const db = require('../models');  // Adjust the path based on your project structure

module.exports = function (app) {
  app.use('/api', router);
};

/**
 * Return all plants in the database.
 * @return 
 *   Array of objects with properties [id, name, mac, location, created_at, updated_at]
 */
router.get('/plants', async function(req, res, next) {
  try {
    const result = await db.Plant.findAll();
    const plants = result.map(obj => obj.toJSON());
    res.json(plants);
  } catch (error) {
    next(error);
  }
});

/**
 * Return a single plant by id.
 * @return
 *   Single Object with properties [id, name, mac, location, created_at, updated_at]
 */
router.get('/plant/:plant_id', async function(req, res, next) {
  try {
    const plant = await db.Plant.findOne({
      where: { id: req.params.plant_id },
      order: [[ "createdAt", "DESC" ]]
    });

    if (!plant) {
      return res.status(404).json({ error: 'Plant not found' });
    }
    res.json(plant);
  } catch (error) {
    next(error);
  }
});

/**
 * Add a new plant to the database.
 * @return
 *   The newly created plant object
 */
router.post('/plants', async function(req, res, next) {
  try {
    const { monitorId, name, location, MAC, createdAt, updatedAt } = req.body;
    const newPlant = await db.Plant.create({
      id: monitorId,
      name,
      location,
      mac: MAC,
      createdAt,
      updatedAt
    });
    res.status(201).json(newPlant);
  } catch (error) {
    next(error);
  }
});
