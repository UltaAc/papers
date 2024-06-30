shell_rs_root = meson.current_source_dir()

if gobject_introspection_dep.found()
  gir_dir = shell_rs_root / 'gir-files'

  # required to make sure the bindings have stable formatting
  cargo_fmt = find_program('cargo-fmt', required: true)
  run_target('update-rust-bindings',
    command: [
      shell_rs_root / 'update-bindings.sh',
      libppsshell_gir[0].full_path(),
      libppsview_gir[0].full_path(),
      libppsdocument_gir[0].full_path(),
    ],
    depends: [libppsshell_gir[0], libppsview_gir[0], libppsdocument_gir[0]]
  )
endif

subdir('src')
