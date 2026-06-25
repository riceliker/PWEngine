use std::{cell::RefCell, collections::{BTreeMap, HashMap}, ptr::null, rc::Rc};

use sdl3_image_sys::image::IMG_Load;
use sdl3_sys::{rect::SDL_FRect, render::{SDL_CreateTextureFromSurface, SDL_DestroyTexture, SDL_RenderClear, SDL_RenderPresent, SDL_RenderTexture, SDL_Renderer, SDL_SetRenderDrawColor, SDL_SetTextureScaleMode, SDL_Texture}, surface::{self, SDL_SCALEMODE_LINEAR, SDL_Surface}};

use crate::{utils::log::PWELog};

pub trait PWEScene 
{
    /* 
     *  Any Engine Event will be sent here
     */
    fn event(&mut self);
    fn render(&mut self, render: *mut SDL_Renderer);
}

pub struct PWECanvasScene
{
    canvas_map: HashMap<String, *mut SDL_Surface>,
    canvas_index: BTreeMap<i32, Vec<String>>,
}
impl PWECanvasScene
{
    pub fn new() -> Self
    {
        return Self { canvas_map: HashMap::new(), canvas_index: BTreeMap::new() };
    }
    pub fn add_surface(&mut self, name: &str, z_index: i32, surface: *mut SDL_Surface)
    {
        self.canvas_map.insert(String::from(name), surface);
        self.canvas_index.entry(z_index).or_default().push(String::from(name));
    }

}
impl PWEScene for PWECanvasScene 
{
    fn event(&mut self)
    {
        
    }
    
    fn render(&mut self, render: *mut SDL_Renderer) 
    {
        unsafe 
        {
            SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
            SDL_RenderClear(render);
            for (name, surface) in &self.canvas_map
            {
                let texture = SDL_CreateTextureFromSurface(render, *surface);
                let dst_rect = SDL_FRect {
                    x: 0.0, y : 0.0,
                    w: (*texture).w as f32, h: (*texture).h as f32
                };

                SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
                SDL_RenderTexture(render, texture, null(), &dst_rect);
                SDL_DestroyTexture(texture);
            }
            SDL_RenderPresent(render);
        }
        
    }
}