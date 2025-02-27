--[[

    Testing the main reflex engine

    > The script is printing the version to the user.

]]

print("Hello World from Reflex Engine!")
print("Platform: ".. process.platform())
print("Pid: ".. process.pid())

for i, v in pairs(process.versions) do
    print(i, v)
end