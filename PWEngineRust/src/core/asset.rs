/*
 * PWEAsset will manager any assets, until the engine exit.
 * Although it look so big, but you just find something only the name.
 * We suggest developer try to add assets in the preload instead of loop or render.
 */

use std::{cell::{Ref, RefCell}, collections::HashMap, rc::Rc};

use sdl3_sys::*;
use sdl3_ttf_sys::ttf::*;



struct PWEAsset
{
    font: HashMap<String, Rc<RefCell<*mut TTF_Font>>>, 
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
    
}