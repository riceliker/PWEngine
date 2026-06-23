use std::{cell::RefCell, rc::Rc};

use crate::utils::vec::*;
use crate::utils::log::*;
use sdl3_sys::gpu::SDL_GPUDevice;
use sdl3_sys::*;
pub enum PWEWindowMode 
{
    RESIZABLE, FULLSCREEN
}
impl PWEWindowMode 
{
    pub fn as_sdl3(&self) -> video::SDL_WindowFlags
    {
        match self {
            PWEWindowMode::RESIZABLE => return  video::SDL_WINDOW_RESIZABLE,
            PWEWindowMode::FULLSCREEN => return  video::SDL_WINDOW_FULLSCREEN
        }
    }
}
pub struct PWEWindow
{
    name: String,
    device: Option<Rc<RefCell<*mut gpu::SDL_GPUDevice>>>,
    window: Option<Rc<RefCell<*mut video::SDL_Window>>>,
    render: Option<Rc<RefCell<*mut render::SDL_Renderer>>>
}

impl PWEWindow 
{
    pub fn builder(name: &str, device: Rc<RefCell<*mut SDL_GPUDevice>>) -> Self
    {
        return PWEWindow {
            name: String::from(name),
            device: Some(device),
            window: None,
            render: None
        };
    }
    // 3. Create Window
    pub fn create_window(&mut self, window_resolution: Vec2<u32>, mode: PWEWindowMode)
    {
        unsafe 
        {
            let title_c = std::ffi::CString::new(self.name.clone()).expect("Name Error");
            let window_ptr =  video::SDL_CreateWindow(title_c.as_ptr() , window_resolution.x as i32, window_resolution.y as i32, video::SDL_WINDOW_METAL);
            if window_ptr.is_null() 
            {
                PWELog::log_warn("Warning: Can not create window.");
            }
            if !gpu::SDL_ClaimWindowForGPUDevice(self.get_device_ptr(), window_ptr)
            {
                PWELog::log_warn("Claimed device and window was failed.");
            }
            self.window = Some(Rc::new(RefCell::new(window_ptr)));
        }
        
    }
    
    // 4. Create Render
    pub fn create_render(&mut self, logical_resolution: Vec2<u32>)
    {
        unsafe 
        {
            let renderer_ptr = render::SDL_CreateGPURenderer(self.get_device_ptr(), self.get_window_ptr());
            if renderer_ptr.is_null()
            {
                PWELog::log_warn("Create render failed");
            }
            render::SDL_SetRenderLogicalPresentation(renderer_ptr,logical_resolution.x as i32, logical_resolution.y as i32, render::SDL_LOGICAL_PRESENTATION_LETTERBOX);
            self.render = Some(Rc::new(RefCell::new(renderer_ptr)));
        }
    }
    pub fn get_window_ptr(&self) -> *mut video::SDL_Window
    {
        return *self.window.as_ref().expect("You should create new window firstly").borrow();
    }
    pub fn get_device_ptr(&self) -> *mut gpu::SDL_GPUDevice
    {
        return *self.device.as_ref().expect("You should create new device firstly").borrow();
    }
    pub fn get_render_ptr(&self) -> *mut render::SDL_Renderer
    {
        return *self.render.as_ref().expect("You should create new render firstly").borrow();
    }
}