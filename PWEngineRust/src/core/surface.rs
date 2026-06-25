use std::{cell::RefCell, collections::HashMap, rc::Rc};

use sdl3_image_sys::image::IMG_Load;
use sdl3_sys::{rect::SDL_Rect, surface::{SDL_BlitSurface, SDL_DestroySurface, SDL_FillSurfaceRect, SDL_MapSurfaceRGBA, SDL_Surface}, *};
use sdl3_ttf_sys::ttf::{TTF_Font, TTF_RenderText_Blended, TTF_SetFontSize};

use crate::utils::{log::PWELog, vec::{PWEColor, Vec2}};

/*
 * Surface Pool is a class to avoid manager any surface life by your hand.
 * If you have so many surface, try storage in the different pool is a good idea.
 * Use Rc<> help you manger the pool life.
 * Notice: It not thread safety.
 */
pub struct PWESurfacePool
{
    surface_map: HashMap<String, *mut surface::SDL_Surface>,
}

impl PWESurfacePool 
{
    pub fn new() -> Self
    {
        return Self {surface_map: HashMap::new()};
    }
    pub fn get_surface(&self, name: &str) -> Option<*mut SDL_Surface>
    {
        match self.surface_map.get(name) {
            Some(surface) => {
                return Some(*surface)
            },
            None => {
                return None;
            }
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
            self.surface_map.insert(String::from(name), surface);

        }
    }
    pub fn add_surface_by_text(&mut self, name: &str, font: *mut TTF_Font, font_size: u32, color: PWEColor, text: &str)
    {
        unsafe 
        {
            TTF_SetFontSize(font, font_size as f32);
            let text_ptr = std::ffi::CString::new(text).expect("Name Error");
            let surface = TTF_RenderText_Blended(font, text_ptr.as_ptr(), text.len(), color.for_sdl3());
            if surface.is_null()
            {
                PWELog::log_warn("Warning: Surface create failed.");
                return;
            }
            self.surface_map.insert(String::from(name), surface);
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
        match self.surface_map.get(from_name) 
        {
            Some(from_surface) => {
                self.surface_map.insert(String::from(name), *from_surface);
                return;
            },
            None => {
                PWELog::log_warn("Warning: Surface not found.");
                return;
            }
        }
    }
    pub fn clear(&self, name: &str, color: PWEColor)
    {
        unsafe 
        {
            match self.surface_map.get(name) {
                Some(surface) => {
                    let sdl_rect = SDL_Rect {
                    x: 0, y: 0,
                    w: (**surface).w, h: (**surface).h
                    };
                    SDL_FillSurfaceRect(*surface, &sdl_rect,SDL_MapSurfaceRGBA(*surface, color.r, color.g, color.b, color.a));
                }
                None => {
                    PWELog::log_warn("Warning: Surface not found.");
                    return;
                }
            }
            
        }
        
    }
    pub fn draw_subsurface_in_surface(&self, surface_name: &str, sub_surface_name: &str, pos: Vec2<u32>, sub_pos: Vec2<u32>, sub_size: Vec2<u32>)
    {
        unsafe 
        {
            let surface: *mut SDL_Surface;
            match self.surface_map.get(surface_name) {
                Some(temp) => {
                    surface = *temp;
                }
                None => {
                    PWELog::log_warn("Warning: Surface not found.");
                    return;
                }
            }
            let sub_surface: *mut SDL_Surface;
            match self.surface_map.get(sub_surface_name) {
                Some(temp) => {
                    sub_surface = *temp;
                }
                None => {
                    PWELog::log_warn("Warning: Surface not found.");
                    return;
                }
            }

            let src_rect = SDL_Rect { x: sub_pos.x as i32, y: sub_pos.y as i32, w: sub_size.x as i32, h: sub_size.y as i32 };
            let dst_rect = SDL_Rect{ x: pos.x as i32, y: pos.y as i32, w: 0, h: 0 };
            SDL_BlitSurface(sub_surface, &src_rect, surface, &dst_rect);
        }
        
    }
    pub fn draw_rect_in_surface(&mut self, surface_name: &str, rect_pos: Vec2<u32>, rect_size: Vec2<u32>, color: PWEColor)
    {
        let mut surface: *mut SDL_Surface;
        match self.surface_map.get(surface_name)
        {
            Some(temp) =>{
                surface = *temp;
            },
            None =>{
                PWELog::log_warn("Warning: Surface not found.");
                return;
            }
        }
        unsafe 
        {
            let sdl_rect = SDL_Rect {
                x: rect_pos.x as i32, y: rect_pos.y as i32,
                w: rect_size.x as i32, h: rect_size.y as i32,
            };
            SDL_FillSurfaceRect(surface, &sdl_rect,SDL_MapSurfaceRGBA(surface, color.r, color.g, color.b, color.a));
        }
    }
}

impl Drop for PWESurfacePool
{
    fn drop(&mut self) 
    {
        for(key, scene) in &self.surface_map
        {
            if scene.is_null()
            {
                PWELog::log_info(format!("The scene was delete? it maybe have a problem -> Name:{}", key).as_str());
                continue;
            }
            unsafe 
            {
                SDL_DestroySurface(*scene);
            }
        }
    }
}