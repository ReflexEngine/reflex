--[[

    Testing the logger api.

    > Reflex has a logger api at `reflex#logger` which is a table containing logger methods.
    > This will use a method to override the print method.

]]

reflex.logger.info("Hello, World!")
reflex.logger.warn("Hello, World!")
reflex.logger.error("Hello, World!")
reflex.logger.debug("Hello, World!")

reflex.logger.overridePrint()

print("Hello, World!")