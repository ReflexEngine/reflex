--- # Process API
--- 
--- This is the process API which includes information and everything about the process.
process = {}

--- Returns the platform of this computer
---@return string Platform The platform of the computer (aka Operating System)
function process.platform() return "" end

--- Returns the pid of the reflex program
---@return integer Pid The PID of the program.
function process.pid() return 0 end

--- Exits out of the program
---@param code number Status code to exit with
---@return nil nil Returns nil since the application ends and doesnt continue
function process.exit(code) return nil end

--- Returns the reflex version, also can be found at `reflex#version()`
---@return string The resulted version of reflex
function process.version() return "" end

--- Returns the version list used in the runtime
--- @class Versions
process.versions = {}

--- Includes a list of arguments given to the script  
---   
--- These are the arguments after the program `--` like this:  
--- 
--- @example `reflex run test.lua -- <script_args>`
process.argv = {}

--- Reflex Engine Versions Class
--- @class Versions
--- @field reflex string Reflex Version
--- @field lua string Lua Version
--- @field uv string Libuv & Luv version
local versions = {}