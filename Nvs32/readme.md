# CPP Nvs driver
This folder contains an ESP-IDF driver for Nvs written in C++.


## Member Functions
- init(nvs_namespace) - init nvs

- get_instance(void) - get nvs singleton instance

- write( key, const T* input, n_itens) - write blob on nvs

- read(key, T* output, size_t& n_itens) - read blob from nvs

## Using the component
- Creating an instance in main.h inside class Main
```bash
NVS::Nvs& nvs {NVS::Nvs::get_instance()};
``````

- Init the nvs name_space in main.cpp inside main::setup
```bash
status = nvs.init(nspace);
``````
- Use Nvs member functions in your code
```bash
status = nvs.write(key_ssid,ssid,sizeof(ssid));
status = nvs.read(key_ssid,output_ssid,len);
```

- Close Nvs name_space
```bash
nvs.close();
```

## Example
Download an example code from Nvs32/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all MySpiffs component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.






