project('mars', 'c', version : '0.0.1')

# Mount subdirectories
sources = []
tests = []

subdir('tests')
subdir('tests/dummy')
subdir('src')
subdir('src/misc')
subdir('src/net/http')
subdir('src/net/http/encodings')

# Dependencies
unity_dep = subproject('unity').get_variable('unity_dep')

# Include directories
src = include_directories('src')

# Executable
executable('main', ['main.c', sources], include_directories : src)

# Tests
foreach test : tests
    name = test[0]
    file = test[1]
    exe = executable(file, ['tests' / file, sources], include_directories : src, dependencies : unity_dep)
    test(name, exe)
endforeach
