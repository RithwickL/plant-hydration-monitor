const path = require('path'),
    rootPath = path.normalize(path.join(__dirname , '..')),
    env = process.env.NODE_ENV || 'development';

console.log("TEST2: " + env);
let config = {
  development: {
    use_env_variable: false,
    root: rootPath,
    dialect: 'sqlite',
    storage: 'database/plant.db',
    app: {
      name: 'plant-hydration-monitor'
    },
    port: 3000
  },

  test: {
    use_env_variable: false,
    root: rootPath,
    dialect: 'sqlite',
    storage: "database/plant.db",
    app: {
      name: 'plant-hydration-monitor'
    },
    port: 3030
  },

  production: {
    use_env_variable: false,
    root: rootPath,
    dialect: 'sqlite',
    storage: "database/plant.db",
    app: {
      name: 'plant-hydration-monitor'
    },
    port: 3030
  }
};

module.exports = config[env];
