project('mars', 'c')

src = include_directories('src')
sources = [
    'src/misc/buffer/buffer.c',
    'src/net/http/core.c',
]

executable('main', ['main.c', sources], include_directories : src)

unity_dep = subproject('unity').get_variable('unity_dep')
tests = [
    ['TestBuffer', 'test_buffer.c'],
]

foreach test : tests
    name = test[0]
    file = test[1]
    exe = executable(file, ['tests' / file, sources], include_directories : src, dependencies : unity_dep)
    test(name, exe)
endforeach
