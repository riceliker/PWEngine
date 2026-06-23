/*
 * PWEAsset will manager any assets, until the engine exit.
 * Although it look so big, but you just find something only the name.
 * We suggest developer try to add assets in the preload instead of loop or render.
 */

use std::{cell::{Ref, RefCell}, collections::HashMap, rc::Rc};

use sdl3_sys::*;
use sdl3_image_sys::image::IMG_Load;
use sdl3_ttf_sys::ttf::*;

use crate::{core::surface::PWESurface, utils::{log::PWELog, vec::{PWEColor, Vec2}}};

struct PWEAsset
{
    font: HashMap<String, Rc<RefCell<*mut TTF_Font>>>,
    surface: HashMap<String, Rc<RefCell<PWESurface>>>,
    
}

impl PWEAsset
{
    pub fn add_font(&mut self, name: &str, path: &str)
    {
        unsafe 
        {
            let path_ptr = std::ffi::CString::new(path).expect("Name Error");
            let font = TTF_OpenFont(path_ptr.as_ptr(), 16.0);
            self.font.insert(String::from(name), Rc::new(RefCell::new(font)));
        }
    }
    pub fn add_surface_by_image(&mut self, name: &str, path: &str)
    {
        unsafe 
        {
            let path_ptr = std::ffi::CString::new(path).expect("Name Error");
            let surface = IMG_Load(path_ptr.as_ptr());
            if surface.is_null()
            {
                PWELog::log_warn(format!("Texture not found by path:{}",path).as_str());
                return;
            }
            self.surface.insert(String::from(name), Rc::new(RefCell::new(PWESurface::new(surface))));

        }
    }
    pub fn add_surface_by_text(&mut self, name: &str, font_assets_name: String, font_size: u32, color: PWEColor, text: &str)
    {
        unsafe 
        {
            match self.font.get(&font_assets_name) 
            {
                Some(font_ptr) => {
                    let font = *font_ptr.borrow_mut();
                    TTF_SetFontSize(font, font_size as f32);
                    let text_ptr = std::ffi::CString::new(text).expect("Name Error").as_ptr();
                    let surface = TTF_RenderText_Blended(font, text_ptr, text.len(), color.for_sdl3());
                    if surface.is_null()
                    {
                        PWELog::log_warn("Warning: Surface create failed.");
                        return;
                    }
                    self.surface.insert(String::from(name), Rc::new(RefCell::new(PWESurface::new(surface))));
                }
                None => {
                    PWELog::log_warn(format!("Warning: Font not found. Name:{}", font_assets_name).as_str());
                    return;
                }
            }
            
        }
    }
    pub fn add_surface(&mut self, name: &str, size: Vec2<u32>, color: PWEColor)
    {
        unsafe 
        {
            let surface = surface::SDL_CreateSurface(size.x as i32, size.y as i32, pixels::SDL_PIXELFORMAT_RGBA8888);
            if surface.is_null()
            {
                PWELog::log_warn("Warning: Create surface error.");
                return;
            }
            surface::SDL_FillSurfaceRect(surface, std::ptr::null(), color.for_u32());
        }
    }
    pub fn add_surface_by_copy(&mut self, name: &str, from_name: &str)
    {
        let name_ptr = std::ffi::CString::new(from_name).expect("Name Error").as_ptr();
        match self.surface.get(from_name) 
        {
            Some(from_surface) => {
                let copied_surface = {
                    let guard = from_surface.borrow();
                    guard.duplicate()
                };
                self.surface.insert(String::from(name), Rc::new(RefCell::new(copied_surface)));
                return;
            },
            None => {
                return;
            }
        }
    }
}