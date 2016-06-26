# flash_display
Fast 2D rendering engine that takes advantage of data-oriented design.

### Examples
Simple object

```C++
Stage stage(800, 600);

DisplayObject obj;
obj.setWidth(300);
obj.setHeight(100);
obj.setX(20);
obj.setY(20);

stage.addChild(&obj);

stage.start();
```

Textures

```C++
Stage stage(800, 600);

FileLoader loader;
loader.load("texture.jpg");

if (loader.size()) {
    Texture* texture = (Texture*) loader.data();

    Image* image = new Image();
    image->setTexture(texture);
    image->setX(10);
    image->setY(300);
    image->setWidth(100);
    image->setHeight(100);
    stage.addChild(image);
}

stage.start();
```
