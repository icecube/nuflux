doxygen = find_program('doxygen',required:false)

if doxygen.found()
  docs_config = {
    'CPP_SOURCE_DIR' : meson.project_source_root() + '/src',
    'DOCS_SOURCE_DIR' : meson.current_source_dir(),
    'DOXYGEN_XML_DIR' : meson.current_build_dir() + '/doxygen_xml',
    'PROJECT_VERSION' : meson.project_version(),
  }

  Doxyfile = configure_file(
    input : 'Doxyfile.in',
    output : 'Doxyfile',
    configuration : docs_config,
    )

  doxygen_xml += custom_target(doxygen,
      input : [Doxyfile],
      output : ['doxygen'],
      command : ['doxygen', '@INPUT@']
      )

  sphinx_conf = configure_file(
    input : 'conf.py.in',
    output : 'conf.py',
    configuration : docs_config,
    )

  html = custom_target('sphinx',
    depends : doxygen_xml,
    input : [sphinx_conf, 'about.rst', 'development.rst', 'fluxes.rst',
             'installation.rst', 'pythonapi.rst', 'cppapi.rst', 'evaluation.rst',
             'index.rst', 'overview.rst', 'usage.rst' ],
    output : ['html'],
    command : ['sphinx-build', '-c', meson.current_build_dir(), meson.current_source_dir(),
              meson.current_build_dir() + '/html'],
    )

  doc_tarball = custom_target('docs-tarball',
    input : [html],
    output : ['nuflux-docs-html-' + meson.project_version() + '.tar.gz'],
    command : ['tar', 'czvf', '@OUTPUT@', '-C', meson.current_build_dir() + '/html', '.'],
    )

  alias_target('html-tarball', doc_tarball)
else
  warning('Doxygen not found: skipping documentation build')
endif