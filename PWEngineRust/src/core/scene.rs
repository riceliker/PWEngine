use std::{cell::RefCell, collections::{BTreeMap, HashMap}, rc::Rc};

use sdl3_image_sys::image::IMG_Load;
use sdl3_sys::surface::{self, SDL_Surface};

use crate::{core::surface::PWESurface, utils::log::PWELog};

pub trait PWEScene 
{
    /* 
     *  Any Engine Event will be sent here
     */
    fn event();
    fn render();
}

pub struct PWECanvasScene
{
    canvas_map: HashMap<String, Rc<RefCell<PWESurface>>>,
    canvas_index: BTreeMap<i32, Vec<String>>,
}
impl PWECanvasScene
{
    pub fn add_surface(&mut self, name: &str, z_index: i32, surface: *mut SDL_Surface)
    {
        self.canvas_map.insert(String::from(name),
             Rc::new(RefCell::new(
                    PWESurface::new(surface)
                ))
            );
        self.canvas_index.entry(z_index).or_default().push(String::from(name));
    }

}
impl PWEScene for PWECanvasScene 
{
    fn event()
    {
        
    }
    
    fn render() 
    {
        
    }
}