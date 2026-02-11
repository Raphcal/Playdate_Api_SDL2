//
//  events.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef events_h
#define events_h

typedef enum {
    /** La langue du jeu a changé. */
    EventLanguageChanged,
    /** Le joueur de gauche vient d'envoyer des jetons au joueur de droite. */
    EventLeftSideSentChips,
    /** Le joueur de gauche vient d'envoyer une étoile. */
    EventLeftSideSentStar,
    /** Le joueur de droite vient d'envoyer des jetons au joueur de gauche. */
    EventRightSideSentChips,
    /** Événement quand le joueur est au centre, pour éviter de recevoir les jetons de ses propres attaques. */
    EventCenterSideSentChips,
    /** Le joueur de gauche à perdu. */
    EventLeftSideLost,
    /** Le joueur de droite à perdu. */
    EventRightSideLost,
    /** Événement quand le joueur est au centre. */
    EventCenterSideLost,
    /** 1 si le joueur de gauche est proche de perdre, 0 s'il a réussi à se sauver. */
    EventLeftSideMayLose,
    /** 1 si le joueur de droite est proche de perdre, 0 s'il a réussi à se sauver. */
    EventRightSideMayLose,
    /** Événement quand le joueur est au centre. */
    EventCenterSideMayLose,
    /** Le joueur de gauche vient de faire une main. L'argument est le type de main. */
    EventLeftSideMadePokerHand,
    /** Le joueur de gauche à faire disparaître des cartes. L'argument est le nombre de cartes. */
    EventLeftSideRemovedCards,
    /** Le joueur de gauche à faire disparaître des jetons. L'argument est le nombre de jetons. */
    EventLeftSideRemovedChips,
    /** Le joueur de gauche vient de faire un combo. */
    EventLeftSideMadeCombo,
    /** Le jouer de gauche/centre vient de nettoyer entièrement son plateau. */
    EventLeftSideClearedBoard,
    /** Indique que le dialogue affiché actuellement a changé. */
    EventDialogChanged,
    /** Indique que le dialogue est terminé. */
    EventDialogHasEnded,
    /**
     * Called by pdportal web app after serial connection established. Example
     * handler: Update UI to show that the serial connection was successful.
     */
    EventPDPortalOnConnect,
    /**
     * Called by Lua code when serial keepalive fails. Example
     * handler: Update UI to show that the serial has been disconnected.
     */
    EventPDPortalOnDisconnect,
    /**
     * Peer connection established, remote peer connection possible. Example
     * handler: Update UI to show this Playdate's peer ID. Other users can enter
     * the ID to connect.
     * @see https://peerjs.com/docs/#peeron-open
     */
    EventPDPortalOnPeerOpen,
    /**
     * Peer connection destroyed, remote peer connection no longer possible.
     * Example handler: Update UI to show that peer connection is no longer
     * possible. Prompt user to reconnect?
     * @see https://peerjs.com/docs/#peeron-close
     */
    EventPDPortalOnPeerClose,
    /**
     * A remote peer has connected to us via the peer server. The given
     * `remotePeerId` can be used to send data to this peer with
     * `PdPortal.PortalCommand.SendToPeerConn`. Example handler: Start game!
     * @see https://peerjs.com/docs/#peeron-connection
     */
    EventPDPortalOnPeerConnection,
    /**
     * We've connected to a remote peer via the peer server. The given
     * `remotePeerId` can be used to send data to this peer with
     * `PdPortal.PortalCommand.SendToPeerConn`. Example handler: Start game!
     * @see https://peerjs.com/docs/#dataconnection-on-open
     */
    EventPDPortalOnPeerConnOpen,
    /**
     * Connection to a remote peer has been lost. Example handler: End game.
     * @see https://peerjs.com/docs/#dataconnection-on-close
     */
    EventPDPortalOnPeerConnClose,
    /**
     * We've received data from a remote peer. Example handler: Read `payload` and
     * act appropriately based on the `remotePeerId` and current app state. Note
     * that `payload` is JSON-encoded, so you probably want to
     * `local decodedPayload = json.decode(payload)` before use.
     * @see https://peerjs.com/docs/#dataconnection-on-data
     * @see https://sdk.play.date/inside-playdate#M-json
     */
    EventPDPortalOnPeerConnData,
    /** Nombre d'événements. */
    EventCount
} Event;

#endif /* events_h */
