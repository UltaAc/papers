papers_sources = files(
)

resources_conf = configuration_data()
resources_conf.set('APP_ID', app_id)

papers_sources += gnome.compile_resources(
  'pps-resources',
  configure_file(
    input: 'papers.gresource.xml.in',
    output: 'papers.gresource.xml',
    configuration: resources_conf,
  ),
  source_dir: [data_dir, data_build_dir],
  dependencies: metainfo_file,
  c_name: pps_code_prefix.to_lower(),
  export: true,
)

marshal = 'pps-shell-marshal'

marshal_sources = gnome.genmarshal(
  marshal,
  sources: marshal + '.list',
  prefix: 'pps_shell_marshal',
  internal: true,
  extra_args: '--prototypes',
)

papers_sources += marshal_sources

papers_resources = gnome.compile_resources(
  'pps-resources',
  configure_file(
    input: 'papers.gresource.xml.in',
    output: 'papers.gresource.xml',
    configuration: resources_conf,
  ),
  source_dir: [data_dir, data_build_dir],
  dependencies: metainfo_file,
  gresource_bundle: true,
)[0]

config_h.set_quoted('RESOURCES_FILE', papers_resources.full_path())

papers_deps = [
  gdk_pixbuf_dep,
  libaw_dep,
  libppsdocument_dep,
  libppsview_dep,
  m_dep,
]

papers_cflags = [
  '-DPAPERS_COMPILATION',
]

papers_ldflags = common_ldflags


libppsshell = shared_library(
  'ppsshell-' + pps_api_version,
  version: pps_view_version,
  sources: papers_sources,
  include_directories: top_inc,
  dependencies: papers_deps,
  c_args: papers_cflags,
  install: true,
)

# GObject Introspection
if gobject_introspection_dep.found()
  incs = [
    'Gdk-4.0',
    'GdkPixbuf-2.0',
    'Gio-2.0',
    'GLib-2.0',
    'GObject-2.0',
    'Gtk-4.0',
    'Adw-1',
    libppsdocument_gir[0],
    libppsview_gir[0],
  ]

  libppsshell_gir = gnome.generate_gir(
    [libppsshell, libppsview, libppsdocument],
    sources: papers_sources,
    includes: incs,
    nsversion: pps_api_version,
    namespace: 'PapersShell',
    identifier_prefix: pps_code_prefix,
    symbol_prefix: pps_code_prefix.to_lower(),
    export_packages: 'papers-shell-' + pps_api_version,
    extra_args: '-DPAPERS_COMPILATION',
    install: false,
  )
endif
