/* 
 * Window is a class, which manager the SDL window. it control GPU device and SDL renderer
 */


use std::ffi::{CString};

use sdl3::sys::{
    gpu::{SDL_ClaimWindowForGPUDevice, SDL_CreateGPUDevice, SDL_DestroyGPUDevice, SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPUDevice}, init::{SDL_INIT_VIDEO, SDL_Init}, log::SDL_Log, render::{SDL_CreateGPURenderer, SDL_DestroyRenderer, SDL_Renderer}, video::{SDL_CreateWindow, SDL_DestroyWindow, SDL_ShowWindow, SDL_WINDOW_RESIZABLE, SDL_WINDOW_VULKAN, SDL_Window}
};
fn c_str(str : &str) -> CString
{
    return CString::new(str).unwrap();
}

use crate::utils::vec;
pub struct WindowInfo
{
    pub logical_resolution : vec::Vec2<u32>
}
pub struct Window
{
    window : Option<*mut SDL_Window>,
    device : Option<*mut SDL_GPUDevice>,
    renderer : Option<*mut SDL_Renderer>   
}
impl Drop for Window
{
    fn drop(&mut self) 
    {
        unsafe 
        {
            SDL_DestroyRenderer(self.renderer.unwrap());
            SDL_DestroyGPUDevice(self.device.unwrap());
            SDL_DestroyWindow(self.window.unwrap());
        }
    }
}
impl Window
{
    pub fn new() -> Self
    {
        return Window { window: None, device: None, renderer: None };
    }
    // If you want to use it. you must use it first.
    pub fn create_window(&mut self, info : WindowInfo)
    {
        unsafe 
        {
            SDL_Init(SDL_INIT_VIDEO);

            let maybe_null_window  = SDL_CreateWindow(
                c_str("").as_ptr(), 1280, 720,
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
            if maybe_null_window.is_null()
            {
                SDL_Log(c_str("Error: Create window failed").as_ptr());
            }
            self.window = Some(maybe_null_window);

            let maybe_null_device = SDL_CreateGPUDevice(
                SDL_GPU_SHADERFORMAT_SPIRV, true,
                CString::new("vulkan").unwrap().as_ptr());
            if maybe_null_device.is_null()
            {
                SDL_Log(c_str("Error: Create device failed").as_ptr());
            }
            self.device = Some(maybe_null_device);

            if !SDL_ClaimWindowForGPUDevice(self.device.expect("Device Not Found"), self.window.expect("Window Not Found"))
            {
                SDL_Log(c_str("Error: Claimed device and window was failed.").as_ptr());
            }

            let maybe_null_renderer = SDL_CreateGPURenderer(self.device.unwrap(), self.window.unwrap());
            if maybe_null_renderer.is_null()
            {
                SDL_Log(c_str("Error: Create renderer failed").as_ptr());
            }
            self.renderer = Some(maybe_null_renderer);
            SDL_ShowWindow(self.window.unwrap());

        };
    }
}