print("Hello World from Reflex Engine!")
print("Platform: ".. process.platform())
print("Pid: ".. process.pid())

local version = process.versions()


print("Reflex Version: ".. version.reflex)
print("Lua Version: ".. version.lua)

local my_dep = require("my_dep")

my_dep.printHelloWorld()

for i, v in pairs(process.argv) do
    print(i, v)
end