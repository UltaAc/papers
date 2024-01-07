shell_rs_root = meson.current_source_dir()

if enable_introspection
  gir_dir = shell_rs_root / 'gir-files'

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
