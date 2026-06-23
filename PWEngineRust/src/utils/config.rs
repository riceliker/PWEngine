use std::*;

const TEMPLATE_CONFIG: &str = include_str!("./config_template.json");
const CONFIG_PATH: &str = "config.json";
pub struct PWEConfigManager;

impl PWEConfigManager 
{
    fn file_finder() -> serde_json::Map<String, serde_json::Value> {
        let path = path::Path::new(CONFIG_PATH);
        if !path.exists() {
            fs::write(path, TEMPLATE_CONFIG).expect("ERROR: Can not create config.json");
        }

        let file_content = fs::read_to_string(path).expect("ERROR: Can not read in config.json");

        serde_json::from_str(&file_content)
            .expect("ERROR: The config.json struct is terrible. please delete it, engine will create new default.")
    }
    fn save_to_disk(map: &serde_json::Map<String, serde_json::Value>) {
        let json_str = serde_json::to_string_pretty(map)
            .expect("Failed serialize config map to json");
        fs::write(CONFIG_PATH, json_str)
            .expect("ERROR: Can not write config.json");
    }

    pub fn get_i64(yield_name: String, key_name: String) -> Option<i64> {
        let root_map = Self::file_finder();
        let yield_map_variable = root_map.get(&yield_name)?;
        let yield_map = yield_map_variable.as_object()?;
        let value = yield_map.get(&key_name)?;
        value.as_i64()
    }

    pub fn set_i64(yield_name: String, key_name: String, input_value: i64) {
        let mut root_map = Self::file_finder();
        if let Some(yield_map_variable) = root_map.get_mut(&yield_name) {
            if let Some(yield_map) = yield_map_variable.as_object_mut() {
                if let Some(value) = yield_map.get_mut(&key_name) {
                    let new_num = serde_json::Number::from(input_value);
                    *value = serde_json::Value::Number(new_num);
                }
            }
        }
        Self::save_to_disk(&root_map);
    }
    pub fn get_f64(yield_name: String, key_name: String) -> Option<f64> {
        let root_map = Self::file_finder();
        let yield_map_variable = root_map.get(&yield_name)?;
        let yield_map = yield_map_variable.as_object()?;
        let value = yield_map.get(&key_name)?;
        value.as_f64()
    }

    pub fn set_f64(yield_name: String, key_name: String, input_value: f64) {
        let mut root_map = Self::file_finder();
        if let Some(yield_map_variable) = root_map.get_mut(&yield_name) {
            if let Some(yield_map) = yield_map_variable.as_object_mut() {
                if let Some(value) = yield_map.get_mut(&key_name) {
                    if let Some(n) = serde_json::Number::from_f64(input_value) {
                        *value = serde_json::Value::Number(n);
                    }
                }
            }
        }
        Self::save_to_disk(&root_map);
    }
    pub fn get_bool(yield_name: String, key_name: String) -> Option<bool> {
        let root_map = Self::file_finder();
        let yield_map_variable = root_map.get(&yield_name)?;
        let yield_map = yield_map_variable.as_object()?;
        let value = yield_map.get(&key_name)?;
        value.as_bool()
    }

    pub fn set_bool(yield_name: String, key_name: String, input_value: bool) {
        let mut root_map = Self::file_finder();
        if let Some(yield_map_variable) = root_map.get_mut(&yield_name) {
            if let Some(yield_map) = yield_map_variable.as_object_mut() {
                if let Some(value) = yield_map.get_mut(&key_name) {
                    *value = serde_json::Value::Bool(input_value);
                }
            }
        }
    }
    pub fn get_string(yield_name: String, key_name: String) -> Option<String> {
        let root_map = Self::file_finder();
        let yield_map_variable = root_map.get(&yield_name)?;
        let yield_map = yield_map_variable.as_object()?;
        let value = yield_map.get(&key_name)?;
        value.as_str().map(|s| s.to_string())
    }

    pub fn set_string(yield_name: String, key_name: String, input_value: String) {
        let mut root_map = Self::file_finder();
        if let Some(yield_map_variable) = root_map.get_mut(&yield_name) {
            if let Some(yield_map) = yield_map_variable.as_object_mut() {
                if let Some(value) = yield_map.get_mut(&key_name) {
                    *value = serde_json::Value::String(input_value);
                }
            }
        }
        Self::save_to_disk(&root_map);
    }
}