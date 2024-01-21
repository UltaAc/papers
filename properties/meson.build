libppsproperties = static_library(
  'evproperties',
  sources: ['pps-properties.c'],
  include_directories: top_inc,
  dependencies: libppsdocument_dep,
  gnu_symbol_visibility: 'hidden',
)

libppsproperties_dep = declare_dependency(
  include_directories: include_directories('.'),
  dependencies: libppsdocument_dep,
  link_with: libppsproperties,
)

if enable_nautilus
  deps = [
    libppsdocument_dep,
    libnautilus_extension_dep,
  ]

  shared_module(
    'papers-properties-page',
    sources: 'pps-properties-main.c',
    include_directories: top_inc,
    dependencies: deps,
    link_with: libppsproperties,
    install: true,
    install_dir: nautilus_extension_dir,
    gnu_symbol_visibility: 'hidden',
  )
endif
