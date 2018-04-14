#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

struct Hitbox {
    sf::FloatRect rect;
    unsigned int infos;
    
    Hitbox() {
        infos = 0;
    }
    
    Hitbox(sf::FloatRect const& rect, unsigned int i = 0) {
        this->rect = rect;
        this->infos = i;
    }
    
    Hitbox(Hitbox const& copy) {
        this->rect = copy.rect;
        this->infos = copy.infos;
    }
};

/**
 * Représente une frame
 */
struct Frame {
    // Les dimensions
    int width, height;
    
    // La position
    int x, y;
    
    // Le point d'ancrage
    int origineX, origineY;
    
    std::vector<Hitbox> hitboxes;
    
    Frame() {
        width = 0;
        height = 0;
        x = 0;
        y = 0;
        
        origineX = 0;
        origineY = 0;
    }
    
    Frame(int x, int y, int w, int h, int ox, int oy) {
        width = w;
        height = h;
        this->x = x;
        this->y = y;
        
        origineX = ox;
        origineY = oy;
    }
};

int frameAtLocation(int x, int y, std::vector<Frame> const& sequence);
int hitboxAtLocation(int x, int y, int frameIndex, std::vector<Frame> const& sequence);
void displayWelcomMessage();
bool manageEventOfWindow(sf::Event &event, sf::RenderWindow& window);
void manageView(sf::Event &event, sf::View &view, float &velocity, bool &moveView, sf::Vector2f pos, sf::Vector2i &old);

Frame createFrameFromPoints(sf::Vector2f o, sf::Vector2f l, sf::Vector2u max);

int main(int argc, char const** argv) {
    displayWelcomMessage();
    
    sf::RenderWindow preview(sf::VideoMode(200, 200, 32), "Preview", sf::Style::Titlebar | sf::Style::Resize);
    
    // La fenêtre
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Sequence Maker 3000");
    
    preview.setPosition(window.getPosition() + sf::Vector2i(-200, 0));
    
    sf::Font font;
#if defined(__APPLE__)
    font.loadFromFile("/Library/Fonts/Courier New.ttf");
#elif defined(_WIN32)
     font.loadFromFile("Courier New.ttf");
#else
    font.loadFromFile("/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf");
#endif
    sf::Text modeTxt;
    modeTxt.setFont(font);
    modeTxt.setPosition(20, 550);
    
    sf::Text inFileTxt;
    sf::Text outFileTxt;
    
    inFileTxt.setFont(font);
    inFileTxt.setCharacterSize(12);
    outFileTxt.setFont(font);
    outFileTxt.setCharacterSize(12);
    outFileTxt.setPosition(0, 14);

    // La vue du programme
    sf::View view;
    view.setSize(800, 600);
    view.setCenter(400, 300);
    
    // Le chemin du fichier de l'image et de la séquence
    std::string inPath, outPath;
    
    // Le premier arguments est le fichier d'entrée
    if(argc > 1) {
        inPath = argv[1];
    }
    
    // Le deuxième paramètre est le fichier de sortie
    if(argc > 2) {
        outPath = argv[2];
    }
    
    if(inPath == "") {
        std::cout << "Veuillez entrer le chemin vers l'image a charger:";
        std::getline(std::cin, inPath);
    }
    
    sf::Texture texture;
    sf::Sprite sprite;
    
    // On charge la texture
    if(!texture.loadFromFile(inPath)) {
        std::cerr << "Can't load '" << inPath << "' file!" << std::endl;
        return EXIT_FAILURE;
    }
    
    inFileTxt.setString(inPath);
    outFileTxt.setString(outPath);
    
    sprite.setTexture(texture);

    
    // Un tableau de frame représentant la séquence
    std::vector<Frame> sequence;
    
    // La frame actuelle
    Frame actual;
    
    // si f = true, on est en train de dessiner un rectangle jaune
    bool f = false;
    sf::Vector2f o;
    
    // Somme nous en train de déplacer la vue avec la souris et la molette
    bool moveView = false;
    
    // L'ancienne position de la souris
    sf::Vector2i old;
    
    int previewIndex = 0;
    sf::Clock previewClock;
    
    // Le contour de la texture
    sf::RectangleShape bound;
    bound.setPosition(0, 0);
    bound.setSize(sf::Vector2f(texture.getSize().x, texture.getSize().y));
    bound.setFillColor(sf::Color(255, 255, 255, 100));
    
    enum EditorState {
        EDITOR_STATE_SEQUENCE,
        EDITOR_STATE_HITBOXES
    };
    
    float viewVel = 1;
    bool showGrid = true;
    
    EditorState actualState = EDITOR_STATE_SEQUENCE;
    bool save = true;
    
    // Tant que la fenêtre est ouverte
    while(window.isOpen()) {
        sf::Event event;
        
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
        while(window.pollEvent(event)) {
            save = manageEventOfWindow(event, window);
            manageView(event, view, viewVel, moveView, window.mapPixelToCoords(sf::Mouse::getPosition(window), view), old);
            
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::G)
                    showGrid = !showGrid;
                else if(event.key.code == sf::Keyboard::Space) {
                    view.setCenter(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2);
                }
            }
            
            switch(actualState) {
                case EDITOR_STATE_SEQUENCE:
                    switch (event.type) {
                        // Si on appui sur une touche du clavier...
                        case sf::Event::KeyPressed:
                            switch(event.key.code) {
                                case sf::Keyboard::Escape:
                                    if(f)
                                        f = false;
                                    break;
                                case sf::Keyboard::BackSpace:
                                    if(sequence.size() > 0)
                                        sequence.pop_back();
                                    break;
                                case sf::Keyboard::C:
                                    sequence.clear();
                                    break;
                                case sf::Keyboard::Tab:
                                    actualState = EDITOR_STATE_HITBOXES;
                                    f = false;
                                    break;
                                default:
                                    break;
                            }
                            break;
                            
                        // Si on a pesé sur un bouton de la souris
                        case sf::Event::MouseButtonPressed: {
                            switch(event.mouseButton.button) {
                                case sf::Mouse::Left:
                                    f = !f;
                    
                                    if(f) {
                                        o.x = mousePos.x;
                                        o.y = mousePos.y;
                                    }
                                    else
                                        sequence.push_back(createFrameFromPoints(o, mousePos, texture.getSize()));
                                    break;
                                case sf::Mouse::Right: {
                                    int index = frameAtLocation(mousePos.x, mousePos.y, sequence);
                                    
                                    if(index != -1) {
                                        sequence[index].origineX = mousePos.x - sequence[index].x;
                                        sequence[index].origineY = mousePos.y - sequence[index].y;
                                    }
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                            break;

                        default:
                            break;
                    }
                    break;
                case EDITOR_STATE_HITBOXES:
                    switch(event.type) {
                        case sf::Event::KeyPressed:
                            switch(event.key.code) {
                                case sf::Keyboard::Tab:
                                    actualState = EDITOR_STATE_SEQUENCE;
                                    f = false;
                                    break;
                                case sf::Keyboard::Escape:
                                    if(f)
                                        f = false;
                                    break;
                                case sf::Keyboard::BackSpace: {
                                    int index = frameAtLocation(mousePos.x, mousePos.y, sequence);
                                    
                                    if(index != -1) {
                                        if(sequence[index].hitboxes.size() > 0)
                                            sequence[index].hitboxes.pop_back();
                                    }
                                }
                                    break;
                                default:
                                    break;
                            }
                          
                            break;
                            
                        case sf::Event::MouseButtonPressed:
                            switch(event.mouseButton.button) {
                                case sf::Mouse::Left:
                                    f = !f;
                                    
                                    if(f) {
                                        o.x = mousePos.x;
                                        o.y = mousePos.y;
                                    }
                                    else {
                                        int index = frameAtLocation(mousePos.x, mousePos.y, sequence);
                                        
                                        if(index != -1) {
                                            Frame& frame = sequence[index];
                                            
                                            Frame temp = createFrameFromPoints(o, mousePos, texture.getSize());
                                            
                                            Hitbox box(sf::FloatRect(temp.x, temp.y, temp.width, temp.height), frame.hitboxes.size());
                                            
                                            frame.hitboxes.push_back(box);
                                            
                                        }
                                    }
                                    
                                    break;
                                case sf::Mouse::Right:
                                {
                                    // On récupère l'index de la frame
                                    int index = frameAtLocation(mousePos.x, mousePos.y, sequence);
                                    int hitbox = hitboxAtLocation(mousePos.x, mousePos.y, index, sequence);
                                    
                                    if(hitbox != -1) {
                                        // On incrémente
                                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                                            sequence[index].hitboxes[hitbox].infos--;
                                        }
                                        else {
                                            sequence[index].hitboxes[hitbox].infos++;
                                        }
                                    }
                                }
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        
        if(actualState == EDITOR_STATE_SEQUENCE) {
            modeTxt.setString("Animation");
        }
        else {
            modeTxt.setString("Collision");
        }
        
        // Efface le contenu de la fenêtre
        window.clear();
        
        // Assigne la vue
        window.setView(view);
        
        // On dessine le contours de la texture
        window.draw(bound);
        
        // On dessine le sprite
        window.draw(sprite);
        
        // On dessine toutes les frames de la séquences
        for(int i = 0; i < sequence.size(); i++) {
            sf::RectangleShape shape;
            shape.setSize(sf::Vector2f(sequence[i].width, sequence[i].height));
            shape.setPosition(sequence[i].x, sequence[i].y);
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineColor(sf::Color::Green);
            shape.setOutlineThickness(1.0f);
            
            window.draw(shape);
            
            sf::CircleShape origineShape;
            origineShape.setRadius(1);
            origineShape.setFillColor(sf::Color::Red);
            origineShape.setOrigin(0.5, 0.5);
            origineShape.setPosition(sequence[i].x + sequence[i].origineX, sequence[i].y + sequence[i].origineY);
            
            if(actualState == EDITOR_STATE_SEQUENCE) {
                sf::Text text;
                std::stringstream s;
                s << i;
                
                text.setFont(font);
                text.setCharacterSize(10);
                text.setString(s.str());
                
                text.setPosition(sequence[i].x, sequence[i].y);
                window.draw(text);
            }
            window.draw(origineShape);
            
            for(int j = 0; j < sequence[i].hitboxes.size() && actualState == EDITOR_STATE_HITBOXES; j++) {
                if(j != 0)
                    shape.setOutlineColor(sf::Color::Magenta);
                else
                    shape.setOutlineColor(sf::Color::Cyan);
                
                shape.setPosition(sequence[i].hitboxes[j].rect.left, sequence[i].hitboxes[j].rect.top);
                shape.setSize(sf::Vector2f(sequence[i].hitboxes[j].rect.width, sequence[i].hitboxes[j].rect.height));
                
                window.draw(shape);
            
                sf::Text text;
                std::stringstream s;
                s << sequence[i].hitboxes[j].infos;
                
                text.setFont(font);
                text.setCharacterSize(10);
                text.setString(s.str());
                
                text.setPosition(sequence[i].hitboxes[j].rect.left, sequence[i].hitboxes[j].rect.top);
                window.draw(text);
            }
            
        }
        
        // Si on est en train de dessiner un rectangle, on le dessine
        if(f) {
            sf::RectangleShape shape;
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
            
            shape.setPosition(o.x, o.y);
            shape.setSize(sf::Vector2f(mousePos.x - o.x, mousePos.y - o.y));
            shape.setOutlineThickness(1.0f);
            
            if(actualState == EDITOR_STATE_SEQUENCE)
                shape.setOutlineColor(sf::Color::Yellow);
            else if(actualState == EDITOR_STATE_HITBOXES)
                shape.setOutlineColor(sf::Color::Green);
            
            shape.setFillColor(sf::Color(255, 255, 0, 100));
            
            window.draw(shape);
        }
        
        if(showGrid) {
            sf::RectangleShape guide;
            
            guide.setOutlineColor(sf::Color(255, 0, 0, 100));
            guide.setOutlineThickness(0.5f);
            
            guide.setSize(sf::Vector2f(texture.getSize().x, 0));
            guide.setPosition(0, mousePos.y);
            window.draw(guide);
            
            guide.setSize(sf::Vector2f(0, texture.getSize().y));
            guide.setPosition(mousePos.x, 0);
            window.draw(guide);
        }
            
        window.setView(window.getDefaultView());
        window.draw(modeTxt);
        window.draw(inFileTxt);
        window.draw(outFileTxt);
        
        // On affiche la fenêtre
        window.display();
        
        // La fenêtre de preview
        sf::View previewView;
        
        sf::Sprite prevSprite;
        prevSprite.setTexture(texture);
        
        if(previewClock.getElapsedTime().asMilliseconds() > 250) {
            
            if(sequence.size() > 0) {
                previewIndex++;
                previewIndex = previewIndex % sequence.size();
            }
            
            previewClock.restart();
        }
        
        if(sequence.size() > 0) {
            prevSprite.setTextureRect(sf::IntRect(sequence[previewIndex].x, sequence[previewIndex].y, sequence[previewIndex].width, sequence[previewIndex].height));
            prevSprite.setOrigin(sequence[previewIndex].origineX, sequence[previewIndex].origineY);
        }
        
        previewView.setSize(fmax(prevSprite.getGlobalBounds().width, prevSprite.getGlobalBounds().height), fmax(prevSprite.getGlobalBounds().width, prevSprite.getGlobalBounds().height));
        previewView.setCenter(prevSprite.getPosition());
      
        preview.clear();
        
        preview.setView(previewView);
        preview.draw(prevSprite);
        preview.display();
    }
    
    preview.close();
    
    if(!save)
        return EXIT_SUCCESS;
    
    std::cin.clear();
    
    // ici débute la sauvegarde de la séquence
#if 0
    std::cout << "Quel est la durée de cette séquence en milisecondes?" << std::endl;
    unsigned int duration;
    std::cin >> duration;
#endif
    if(outPath == "") {
        std::cout << "Veuillez entrer un fichier de sortie s'il-vous-plait :)" << std::endl;
        std::cin.clear();
        
        getline(std::cin, outPath);
    }
    
    std::ofstream stream;
    
    stream.open(outPath.c_str());
    
    if(!stream.is_open()) {
        std::cerr << "Can't create '" << outPath << "' file!" << std::endl;
        return EXIT_FAILURE;
    }

#if 0
    stream << "duration: " << duration << std::endl;

    stream << "frames{" << std::endl;
#endif
    for(int i = 0; i < sequence.size(); i++) {
        float left, bottom, right, top;
        left = sequence[i].x;
        bottom = sequence[i].y;
        right = sequence[i].x + sequence[i].width;
        top = sequence[i].y + sequence[i].height;


#if 1
        stream << left / (float)texture.getSize().x << " ";
        stream << bottom / (float)texture.getSize().y << " ";
        stream << right / (float)texture.getSize().x << " ";
        stream << top / (float)texture.getSize().y << " ";

#else
        //
        stream << "    frame: ";
        stream << (float)sequence[i].x / (float)texture.getSize().x << " ";
        stream << (float)sequence[i].y / (float)texture.getSize().y << " ";
        stream << (float)sequence[i].width / (float)texture.getSize().x << " ";
        stream << (float)sequence[i].height / (float)texture.getSize().y << " ";
        stream << (float)sequence[i].origineX / (float)texture.getSize().x << " ";
        stream << (float)sequence[i].origineY / (float)texture.getSize().y;
#endif
        stream << std::endl;

#if 0
        if(sequence[i].hitboxes.size() > 0) {
            stream << "    hitbox{" << std::endl;
            for(int j = 0; j < sequence[i].hitboxes.size(); j++) {
                if(j == 0) {
                    stream << "        body: ";
                }
                else {
                    stream << "        hitbox: ";
                }
                
                stream << (float)sequence[i].hitboxes[j].rect.left / (float)texture.getSize().x << " ";
                stream << (float)sequence[i].hitboxes[j].rect.top / (float)texture.getSize().y << " ";
                stream << (float)sequence[i].hitboxes[j].rect.width / (float)texture.getSize().x << " ";
                stream << (float)sequence[i].hitboxes[j].rect.height / (float)texture.getSize().y << " ";
                stream << sequence[i].hitboxes[j].infos;
                stream << std::endl;
            }
            stream << "    }" << std::endl;
        }
#endif
    }
#if 0
    stream << "}";
#endif
    
    return EXIT_SUCCESS;
}

int frameAtLocation(int x, int y, std::vector<Frame> const& sequence) {
    for(int i = 0; i < sequence.size(); i++) {
        sf::IntRect rect;
        rect.left = sequence.at(i).x;
        rect.top = sequence.at(i).y;
        
        rect.width = sequence.at(i).width;
        rect.height = sequence.at(i).height;
        
        if(rect.contains(x, y)) {
            return i;
        }
    }
    
    return -1;
}

int hitboxAtLocation(int x, int y, int frameIndex, std::vector<Frame> const& sequence) {
    if(frameIndex != -1) {
        for(int i = sequence[frameIndex].hitboxes.size() - 1; i > 0; i--) {
            if(sequence[frameIndex].hitboxes[i].rect.contains(x, y)) {
                return i;
            }
        }
    }
    
    return -1;
}

void displayWelcomMessage() {
    // Description du programme
    std::cout << "Bienvenue dans Sequence Maker 3000, Nathan Giraldeau 2013" << std::endl;
    std::cout << "Commandes:" << std::endl;
    std::cout << "Zoom - Utiliser la molette" << std::endl;
    std::cout << "Déplacer camera - Appuyer sur la molette" << std::endl;
    std::cout << "Supprimer le dernier élément placé - backspace" << std::endl;
    std::cout << "Supprimer la séquence - c" << std::endl;
    std::cout << "Modifier le point d'ancrage - Bouton droit de la souris" << std::endl;
    std::cout << "Annuler la sélection - Échapper" << std::endl;
    std::cout << "Sélection - Bouton gauche de la souris" << std::endl;
    std::cout << "Quitter - Retour" << std::endl;
    
}

bool manageEventOfWindow(sf::Event &event, sf::RenderWindow& window) {
    if(event.type == sf::Event::Closed) {
        window.close();
        return false;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        window.close();
    }
    
    return true;
}

void manageView(sf::Event &event, sf::View &view, float &vel, bool &moveView, sf::Vector2f pos, sf::Vector2i &old) {
    switch(event.type) {
        case sf::Event::KeyPressed:
            switch(event.key.code) {
                case sf::Keyboard::Left:
                    view.move(-vel, 0);
                    break;
                case sf::Keyboard::Right:
                    view.move(vel, 0);
                    break;
                case sf::Keyboard::Up:
                    view.move(0, -vel);
                    break;
                case sf::Keyboard::Down:
                    view.move(0, vel);
                    break;
                case sf::Keyboard::LShift:
                    vel = 10;
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::MouseMoved:
            
            if(moveView)
                view.move((event.mouseMove.x - old.x) * -1, (event.mouseMove.y - old.y) * -1);
            
            old.x = event.mouseMove.x;
            old.y = event.mouseMove.y;
            
            break;
        case sf::Event::KeyReleased:
            if(event.key.code == sf::Keyboard::LShift) {
                vel = 1;
            }
            break;
        case sf::Event::MouseButtonPressed:
            if(event.mouseButton.button == sf::Mouse::Middle) {
                moveView = true;
            }
            break;
        case sf::Event::MouseButtonReleased:
            if(event.mouseButton.button == sf::Mouse::Middle) {
                moveView = false;
            }
            break;
        case sf::Event::MouseWheelMoved:
            view.zoom(1 + (event.mouseWheel.delta * 0.01));
            break;
        default:
            break;
    }
}

Frame createFrameFromPoints(sf::Vector2f o, sf::Vector2f l, sf::Vector2u max) {
    Frame actual;
    // On s'assure que le premier point est bon
    if(o.x > max.x) {
        o.x = max.x;
    }
    
    if(o.y > max.y) {
        o.y = max.y;
    }
    
    if(o.x < 0) {
        o.x = 0;
    }
    
    if(o.y < 0) {
        o.y = 0;
    }
    
    // On s'assure que le deuxième point soit bien placé
    if(l.x > max.x) {
        l.x = max.x;
    }
    
    if(l.y > max.y) {
        l.y = max.y;
    }
    
    if(l.x < 0) {
        l.x = 0;
    }
    
    if(l.y < 0) {
        l.y = 0;
    }
    
    if(o.x < l.x)
        actual.x = o.x;
    else
        actual.x = l.x;
    
    if(o.y < l.y)
        actual.y = o.y;
    else
        actual.y = l.y;
    
    actual.width = abs(l.x - o.x);
    actual.height = abs(l.y - o.y);
    
    return actual;
}