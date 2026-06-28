fn main() {
    let header = "./lib/PWEngineCAPI.h";

    let bindings = bindgen::builder()
        .header(header)
        .clang_arg("-I./lib")
        .clang_arg("-std=c23")
        .generate()
        .expect("bindgen failed");

    let out_path = std::path::PathBuf::from(std::env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("write bindings.rs failed");

    println!("cargo:rustc-link-search=./lib");
    println!("cargo:rustc-link-lib=dylib=PWEngineCAPI");

    println!("cargo:rustc-link-arg=-Wl,-rpath,@executable_path/../../lib");
}
