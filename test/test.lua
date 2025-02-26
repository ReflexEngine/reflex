--[[

    Testing the main reflex engine

    > The script is printing the version to the user.

]]

print("Hello World from Reflex Engine!")
print("Platform: ".. process.platform())
print("Pid: ".. process.pid())

local version = process.versions()

print("Reflex Version: ".. version.reflex)
print("Lua Version: ".. version.lua)