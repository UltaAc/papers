papers_sources = files(
  'pps-bookmarks.c',
  'pps-find-sidebar.c',
  'pps-history.c',
  'pps-keyring.c',
  'pps-message-area.c',
  'pps-metadata.c',
  'pps-progress-message-area.c',
  'pps-utils.c',
  'pps-document-view.c',
  'pps-window-title.c',
  'pps-search-context.c',
  'pps-search-result.c',
  'pps-sidebar-annotations.c',
  'pps-sidebar-bookmarks.c',
  'pps-sidebar-page.c',
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

if enable_dbus
  gdbus = 'pps-gdbus'

  papers_sources += gnome.gdbus_codegen(
    gdbus + '-generated',
    gdbus + '.xml',
    interface_prefix: pps_namespace,
    namespace: pps_code_prefix,
    object_manager: true,
    autocleanup: 'objects',
  )
endif

if enable_keyring
  papers_deps += libsecret_dep
endif

enum_types = 'ev-shell-type-builtins'
enum_header_prefix = '''
#include <papers-document.h>
'''

headers = files(
  'pps-document-view.h',
  'pps-metadata.h',
  'pps-sidebar-page.h',
  'pps-search-context.h',
  'pps-search-result.h',
  'pps-utils.h',
  'pps-find-sidebar.h',
  'pps-sidebar-annotations.h',
  'pps-progress-message-area.h',
  'pps-message-area.h',
  'pps-keyring.h',
)

enum_sources = gnome.mkenums_simple(
  enum_types,
  sources: headers,
  header_prefix: enum_header_prefix,
  decorator: 'PPS_PUBLIC',
  install_header: false,
)

libppsshell = shared_library(
  'ppsshell-' + pps_api_version,
  version: pps_view_version,
  sources: papers_sources + enum_sources,
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
    sources: papers_sources + headers + [enum_sources[1]],
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
