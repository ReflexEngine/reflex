--[[

    Testing the lua arguments script system

    > Provide script arguments after `--` in the commandline
    > Example: `reflex run test_args.lua <reflex_args> -- <lua_args>`

]]

for i, v in pairs(process.argv) do
    print(i, v)
end