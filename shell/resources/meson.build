resources_conf = configuration_data()
resources_conf.set('APP_ID', app_id)

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
