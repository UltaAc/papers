papers_sources = files(
  'pps-annotation-properties-dialog.c',
  'pps-annotations-toolbar.c',
  'pps-application.c',
  'pps-bookmarks.c',
  'pps-file-monitor.c',
  'pps-find-sidebar.c',
  'pps-history.c',
  'pps-keyring.c',
  'pps-message-area.c',
  'pps-metadata.c',
  'pps-password-view.c',
  'pps-progress-message-area.c',
  'pps-properties-dialog.c',
  'pps-properties-view.c',
  'pps-properties-fonts.c',
  'pps-properties-license.c',
  'pps-toolbar.c',
  'pps-thumbnail-item.c',
  'pps-utils.c',
  'pps-window.c',
  'pps-window-title.c',
  'pps-search-box.c',
  'pps-sidebar-annotations.c',
  'pps-sidebar-attachments.c',
  'pps-sidebar-bookmarks.c',
  'pps-sidebar.c',
  'pps-sidebar-layers.c',
  'pps-sidebar-links.c',
  'pps-sidebar-page.c',
  'pps-sidebar-thumbnails.c',
  'pps-stack-switcher.c',
  'pps-zoom-action.c',
  'main.c',
)

papers_sources += gnome.compile_resources(
  'pps-resources',
  'papers.gresource.xml',
  source_dir: data_dir,
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
  libppsmisc_dep,
  libppsdocument_dep,
  libppsview_dep,
  libppsproperties_dep,
  libdazzle_dep,
  m_dep,
]

papers_cflags = [
  '-DBINDIR="@0@"'.format(pps_bindir),
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

papers = executable(
  'papers',
  sources: papers_sources,
  include_directories: top_inc,
  dependencies: papers_deps,
  c_args: papers_cflags,
  install: true,
)

if enable_dbus
  papersd_sources = files('pps-daemon.c')

  gdbus = 'pps-daemon-gdbus'

  # The D-Bus interface names are in lowercase, which does not match
  # the application name nor the namespace. However, we keep the
  # interface prefix in lowercase for backward compatibility.
  papersd_sources += gnome.gdbus_codegen(
    gdbus + '-generated',
    gdbus + '.xml',
    interface_prefix: pps_namespace.to_lower(),
    namespace: pps_code_prefix,
    object_manager: true,
    autocleanup: 'objects',
  )

  papersd_deps = [
    gio_dep,
    gio_unix_dep,
  ]

  papersd_cflags = [
    '-DBINDIR="@0@"'.format(pps_bindir),
    '-DPAPERS_COMPILATION',
  ]

  papersd = executable(
    'papersd',
    papersd_sources,
    include_directories: top_inc,
    dependencies: papersd_deps,
    c_args: papersd_cflags,
    install: true,
    install_dir: pps_libexecdir,
  )
endif
