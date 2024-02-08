if not get_option('pymodule')
  test(
    'test_basic',
    executable('test_basic',
      'test_basic.cxx',
      dependencies: [photospline,cfitsio],
      include_directories: inc,
      link_with: libnuflux
    ),
    env: test_env,
  )

  test(
    'readme',
    executable(
      'README',
      custom_target(
        input: ['../README.md'],
        output: ['README.cxx'],
        command: ['awk', '/END CODE/ {p=0};p;/BEGIN CODE/ {p=1}','@INPUT@'],
        capture: true,
      ),
      dependencies: [photospline,cfitsio],
      include_directories: inc,
      link_with: libnuflux
    ),
    env: test_env,
  )
  meson.add_devenv(test_env)
endif

if test_python
  fs = import('fs')
  pytest_env = environment(test_env)
  pytest_env.append('PYTHONPATH', meson.source_root() / 'src')
  pytest_env.append('PYTHONPATH', fs.parent(pymodule.full_path()))
  test('test_fluxes',
    python, args: [files('test_fluxes.py'), '--tap'],
    env: pytest_env,
    protocol: 'tap',
    depends: pymodule)
  test('doctest',
    python, args: ['-m', 'doctest', files('../README.md')],
    env: pytest_env,
    depends: pymodule)
  meson.add_devenv(pytest_env)
endif
