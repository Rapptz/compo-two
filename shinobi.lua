-- the default shinobi.lua file

-- the project settings
project.name = "compo-two";

-- compiler and linker settings
-- with release and debug configuration
if compiler.name == "g++" or compiler.name == "clang++" then
    compiler.flags = { "-std=c++11", "-pedantic", "-pedantic-errors", "-Wextra", "-Wall", "-O2" };
    linker.flags = { "-static" };

    if compiler.name == "g++" then
        table.insert(linker.flags, "-static-libstdc++");
    end

    if config.release then
        linker.libraries = { "-lsfml-graphics", "-lsfml-audio", "-lsfml-window", "-lsfml-network", "-lsfml-system" };
        table.insert(compiler.flags, "-DNDEBUG");
    else
        linker.libraries = { "-lsfml-graphics-d", "-lsfml-audio-d", "-lsfml-window-d", "-lsfml-network-d", "-lsfml-system-d" };
        table.insert(compiler.flags, "-g");
    end

    if os.name == "windows" then
        table.insert(linker.libraries, "-lsfml-main");
    end

    -- lua support
    table.insert(linker.libraries, "-llua");;
-- no planned MSVC support
-- elseif compiler.name == "msvc" then
--     compiler.flags = { "/Za", "/EHsc", "/W3", "/Gm" };

--     if config.release then
--         table.insert(compiler.flags, "/DNDEBUG");
--         table.insert(compiler.flags, "/O2");
--     else
--         table.insert(compiler.flags, "/Od");
--         table.insert(compiler.flags, "/ZI");
--         linker.flags = { "/DEBUG" }
--     end
end

-- include paths
compiler.paths = { ".", "libs", "game" };

-- directory information
directory.build = "bin";
directory.object = "obj";

-- files to compile
files = os.glob("game/*.cpp");
table.insert(files, "libs/pugixml.cpp");
table.insert(files, "main.cpp");
