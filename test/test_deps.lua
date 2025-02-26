--[[

    Testing the Reflex engine libraries system.

    > Libraries are loaded from `reflex_deps` folder
    > so if using this create a sample library at reflex_deps/<library_name>.lua

]]

local libraryName = "my_dep"
local library = require("my_dep")

library.printHelloWorld()