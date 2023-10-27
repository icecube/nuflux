test_env = {'NUFLUX_DATA': meson.project_source_root()+'/src/nuflux/data'}
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
