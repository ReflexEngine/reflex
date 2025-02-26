--- # Reflex 0.0.1
--- 
--- This is the object that includes every Reflex function, method, etc.
--- For managing your application
--- 
--- This simplifies using methods so you don't have to create the same methods
--- over and over
--- 
--- Reflex Team
reflex = {}

--- Displays the reflex version
--- @return string version Reflex Version
function reflex.version() return "" end

--- # Reflex Environment Manager
--- 
--- This is the reflex environment manager, everything here is handled by the C api
--- You can get, set, or load env files (loads System's Environment)
env = {}

--- Load a specific .env file from the filesystem.
function env.load() end

--- Resolves a key from the environment then returns the value of it
---@param key string Key from the environment
---@return string result Resolved value from the environment
function env.get(key) return "" end

--- Replaces a key from the environment
--- This doesn't change the actual environment in the system nor in the .env file
--- 
--- NOTE: Passing nil as value will remove the key.
---@param key string Represents the key of the environment variable
---@param value string|nil Represents the value of the environment variable, nil for removal
---@return boolean
function env.set(key, value) return true end

package.reflex_path = ""

--- # Reflex Logger API
--- 
--- Provides logging materials other than print. 
--- Print can be overriden using `reflex.logger#overridePrint` to use `reflex.logger#info` instead of normal print.
reflex.logger = {}

--- Prints a information message to the console
--- `INFO: <message>`
---@param message string
---@return true
function reflex.logger.info(message) return true end

--- Prints a warning message to the console
--- `WARN: <message>`
---@param message string
---@return true
function reflex.logger.warn(message) return true end

--- Prints a error message to the console
--- `ERROR: <message>`
---@param message string
---@return true
function reflex.logger.error(message) return true end

--- Prints a debugging message to the console
--- `DEBUG: <message>`
---@param message string
---@return true
function reflex.logger.debug(message) return true end

--- Overrides the print method globally so it uses `logger#info`
---@return boolean result Returns true if successful, false if not.
function reflex.logger.overridePrint() return true end