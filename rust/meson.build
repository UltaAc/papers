rust_binding_root = meson.current_source_dir()

cargo_fmt = find_program('cargo-fmt', required: get_option('profile') == 'devel')
if gobject_introspection_dep.found() and cargo_fmt.found()
  gir_dir = rust_binding_root / 'gir-files'

  run_target('update-rust-bindings',
    command: [
      rust_binding_root / 'update-bindings.sh',
      libppsview_gir[0].full_path(),
      libppsdocument_gir[0].full_path(),
    ],
    depends: [libppsview_gir[0], libppsdocument_gir[0]]
  )
endif
