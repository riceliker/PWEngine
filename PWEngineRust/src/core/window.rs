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
    device: Rc<RefCell<*mut gpu::SDL_GPUDevice>>,
    window: Option<*mut video::SDL_Window>,
    render: Option<*mut render::SDL_Renderer>
}

impl PWEWindow 
{
    pub fn builder(name: &str, device: Rc<RefCell<*mut SDL_GPUDevice>>) -> PWEWindow
    {
        return PWEWindow {
            name: String::from(name),
            device: device,
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
            if !gpu::SDL_ClaimWindowForGPUDevice(*self.device.as_ref().borrow_mut(), window_ptr)
            {
                PWELog::log_warn("Claimed device and window was failed.");
            }
            self.window = Some(window_ptr);
        }
        
    }

    // 4. Create Render
    pub fn create_render(&mut self, logical_resolution: Vec2<u32>)
    {
        unsafe 
        {
            let renderer_ptr = render::SDL_CreateGPURenderer(*self.device.as_ref().borrow_mut(), self.get_window_ptr());
            if renderer_ptr.is_null()
            {
                PWELog::log_warn("Create render failed");
            }
            render::SDL_SetRenderLogicalPresentation(renderer_ptr,logical_resolution.x as i32, logical_resolution.y as i32, render::SDL_LOGICAL_PRESENTATION_LETTERBOX);
            self.render = Some(renderer_ptr);
        }
    }
    pub fn show_window(&self)
    {
        unsafe 
        {
            video::SDL_ShowWindow(self.get_window_ptr());
            video::SDL_SetWindowPosition(self.get_window_ptr(),  video::SDL_WINDOWPOS_CENTERED, video::SDL_WINDOWPOS_CENTERED);
        }
    }
    pub fn get_window_ptr(&self) -> *mut video::SDL_Window
    {
        return *self.window.as_ref().expect("You should create new window firstly");
    }
    pub fn get_render_ptr(&self) -> *mut render::SDL_Renderer
    {
        return *self.render.as_ref().expect("You should create new render firstly");
    }
}