--[[

    Testing the environment system of Reflex

    > System environments are loaded automatically
    > `.env` files are loaded by a method. Check the `# Loading .env` section.

]]

-- Using system environments
print(env.get("PATH"))

-- # Loading .env
env.load()

print(env.get("token"))