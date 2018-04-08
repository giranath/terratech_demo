#ifndef DEF_NETWORKING_CONSTANT_HPP
#define DEF_NETWORKING_CONSTANT_HPP

enum
{
    PACKET_SERVER_PUBLIC_KEY,         // Le serveur envoie sa clé publique
    PACKET_SETUP_ENCRYPTION_KEY,      // Le client envoie sa clé AES
    PACKET_SETUP_CHUNK,               // Le serveur envoie plusieurs chunks
    PACKET_SETUP_FLYWEIGHTS,          // Le serveur envoie les types d'unités
    PACKET_SPAWN_UNITS,               // Le client doit ajouter de nouveaux unités
    PACKET_UPDATE_UNITS,              // Le client doit mettre à jour des unités
    PACKET_UPDATE_TARGETS,            // Le client modifie le target d'un unité
    PACKET_PLAYER_ID,                 // Le serveur transfert le player id du joueur
    PACKET_GAME_STATE_CHANGED,        // Le client doit mettre à jour l'état du jeu (en attente, en cours, terminée)
};
#endif
