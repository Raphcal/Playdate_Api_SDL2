//
//  achievements.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 03/04/2025.
//

#include "achievements.h"

const MELAchievementData kAchievementData = (MELAchievementData) {
    .cardPath = "card-achievements",
    .iconPath = "icon",
    .achievements = {
        .count = AchievementCount,
        .memory = (MELAchievement[AchievementCount]) {
            {
                .id = "story-ceres-end",
                .name = {
                    .text = {
                        "Crusty and delicious!",
                        "Doré et croustillant !",
                        "サクサクで美味しい！"
                    }
                },
                .description = {
                    .text = {
                        "Finish Ceres story.",
                        "Terminer l'histoire de Cérès.",
                        "セレスのストーリーをクリアする。",
                    }
                },
                .scoreValue = 50,
                .icon = "sprites/sprite-icon/0",
            },
            {
                .id = "story-caroline-end",
                .name = {
                    .text = {
                        "Exam passed!",
                        "Examen réussi !",
                        "試験合格！"
                    }
                },
                .description = {
                    .text = {
                        "Finish Caroline story.",
                        "Terminer l'histoire de Caroline.",
                        "キャロラインのストーリーをクリアする。",
                    }
                },
                .scoreValue = 60,
                .icon = "sprites/sprite-icon/1",
            },
            {
                .id = "story-clovis-end",
                .name = {
                    .text = {
                        "Lost and Found",
                        "Perdu mais retrouvé",
                        "見つけた！",
                    }
                },
                .description = {
                    .text = {
                        "Finish Clovis story.",
                        "Terminer l'histoire de Clovis.",
                        "クロヴィスのストーリーをクリアする。",
                    }
                },
                .scoreValue = 60,
                .icon = "sprites/sprite-icon/2",
            },
            {
                .id = "story-belleplume-end",
                .name = {
                    .text = {
                        "Seeking the Truth",
                        "Action ou Vérité",
                        "真実を求めて",
                    }
                },
                .description = {
                    .text = {
                        "Finish Finepen story.",
                        "Terminer l'histoire de Belleplume.",
                        "ミツペンのストーリーをクリアする。",
                    }
                },
                .descriptionLocked = {
                    .text = {
                        "Finish ???? story.",
                        "Terminer l'histoire de ????.",
                        "？？？のストーリーをクリアする。",
                    }
                },
                .scoreValue = 70,
                .icon = "sprites/sprite-icon/3",
            },
            {
                .id = "story-crocoford-end",
                .name = {
                    .text = {
                        "Plan Comes Together",
                        "Plan Sans Accroc",
                        "計画どおり",
                    }
                },
                .description = {
                    .text = {
                        "Finish Crocoford story.",
                        "Terminer l'histoire de Crocoford.",
                        "クロコフォードのストーリーをクリアする。",
                        
                    }
                },
                .descriptionLocked = {
                    .text = {
                        "Finish ???? story.",
                        "Terminer l'histoire de ????.",
                        "？？？のストーリーをクリアする。",
                    }
                },
                .scoreValue = 80,
                .icon = "sprites/sprite-icon/4",
            },
            {
                .id = "story-melusine-end",
                .name = {
                    .text = {
                        "Happy Reunion",
                        "Joyeuses Retrouvailles",
                        "うれしい再会",
                    }
                },
                .description = {
                    .text = {
                        "Finish Melusine story.",
                        "Terminer l'histoire de Mélusine.",
                        "メリュジーヌのストーリーをクリアする。",
                    }
                },
                .descriptionLocked = {
                    .text = {
                        "Finish ???? story.",
                        "Terminer l'histoire de ????.",
                        "？？？のストーリーをクリアする。",
                    }
                },
                .scoreValue = 100,
                .icon = "sprites/sprite-icon/5",
            },
            {
                .id = "score-attack-first",
                .name = {
                    .text = {
                        "World Stage",
                        "Scène Internationale",
                        "世界デビュー",
                    }
                },
                .description = {
                    .text = {
                        "Play your first game in Score Attack mode.",
                        "Jouer une partie en mode Score Attack.",
                        "スコアアタックモードを初めてプレイする。",
                    }
                },
                .scoreValue = 20,
            },
            {
                .id = "score-attack-100000",
                .name = {
                    .text = {
                        "Good Start!",
                        "Bon Début !",
                        "いいスタート！",
                    }
                },
                .description = {
                    .text = {
                        "Reach 100,000 points in Score Attack mode.",
                        "Atteindre 100 000 points en mode Score Attack mode.",
                        "スコアアタックモードで10万点を獲得する。",
                    }
                },
                .scoreValue = 30,
            },
            {
                .id = "score-attack-500000",
                .name = {
                    .text = {
                        "It's Serious Now!",
                        "Les Choses Sérieuses!",
                        "本気！"
                    }
                },
                .description = {
                    .text = {
                        "Reach 500,000 points in Score Attack mode.",
                        "Atteindre 500 000 points en mode Score Attack mode.",
                        "スコアアタックモードで50万点を獲得する。",
                    }
                },
                .scoreValue = 70,
            },
            {
                .id = "arcade-first",
                .name = {
                    .text = {
                        "Insert Coin",
                        NULL,
                        "コイン投入"
                    }
                },
                .description = {
                    .text = {
                        "Play your first game in Arcade mode.",
                        "Jouer une partie en mode Arcade.",
                        "アーケードを初めてプレイする。",
                    }
                },
                .scoreValue = 20,
            },
            {
                .id = "arcade-speed-up",
                .name = {
                    .text = {
                        "Gotta Go Fast!",
                        "Plus vite !",
                        "はやく行こう！",
                    }
                },
                .description = {
                    .text = {
                        "Defeat 3 opponents in a row without losing.",
                        "Battre 3 adversaires à la suite sans perdre.",
                        "3人の相手に連続で勝利する。"
                    }
                },
                .scoreValue = 40,
            },
            {
                .id = "arcade-end",
                .name = {
                    .text = {
                        "Arcade Champion",
                        NULL,
                        "アーケードチャンピオン",
                    }
                },
                .description = {
                    .text = {
                        "Defeat Crocoford in arcade mode.",
                        "Battre Crocoford en mode arcade.",
                        "アーケードモードでクロコフォードに勝つ。"
                    }
                },
                .scoreValue = 70,
            },
            {
                .id = "game-victory-1",
                .name = {
                    .text = {
                        "First Victory",
                        "Première Victoire",
                        "初勝利！",
                    }
                },
                .description = {
                    .text = {
                        "Win your first game.",
                        "Remporter votre première victoire.",
                        "初めて勝利する。",
                    }
                },
                .scoreValue = 10,
            },
            {
                .id = "game-victory-20",
                .name = {
                    .text = {
                        "Huge Success",
                        "Venu Pour Gagner",
                        "大成功",
                    }
                },
                .description = {
                    .text = {
                        "Win 20 games.",
                        "Remporter 20 victoires.",
                        "20回勝利する。",
                    }
                },
                .scoreValue = 40,
            },
            {
                .id = "game-defeat-1",
                .name = {
                    .text = {
                        "Learning Experience",
                        "Apprendre à la dure",
                        "負けから学ぶ",
                    }
                },
                .description = {
                    .text = {
                        "Every loss is a step towards improvement!",
                        "Chaque défaite permet de progresser !",
                        "負けも経験のうち！"
                    }
                },
                .scoreValue = 10,
            },
            {
                .id = "chain-x2",
                .name = {
                    .text = {
                        "2x chain",
                        "2x combo",
                        "2連鎖",
                    }
                },
                .description = {
                    .text = {
                        "Achieve a x2 chain.",
                        "Réussir un combo x2.",
                        "2連鎖を達成する。",
                    }
                },
                .scoreValue = 10,
            },
            {
                .id = "chain-x4",
                .name = {
                    .text = {
                        "4x chain",
                        "4x combo",
                        "4連鎖",
                    }
                },
                .description = {
                    .text = {
                        "Achieve a x4 chain.",
                        "Réussir un combo x4.",
                        "4連鎖を達成する。",
                    }
                },
                .scoreValue = 20,
            },
            {
                .id = "chain-star",
                .name = {
                    .text = {
                        "Shooting Star",
                        "Étoile Filante",
                        "流れ星",
                    }
                },
                .description = {
                    .text = {
                        "Send a star to your opponent.",
                        "Envoyer une étoile à l'adversaire.",
                        "敵に星を送る。"
                    }
                },
                .scoreValue = 70,
            },
            {
                .id = "gameplay-jackpot",
                .name = {
                    .text = {
                        "777 Jackpot",
                        "777 Jackpot",
                        "７７７ジャックポット",
                    }
                },
                .description = {
                    .text = {
                        "Match three 7 cards."
                        "Faire un brelan de 7",
                        "7のスリーカードを揃える。",
                    }
                },
                .scoreValue = 10,
            },
            {
                .id = "gameplay-five-kind",
                .name = {
                    .text = {
                        "Beyond Four of a Kind",
                        "Plus que carré",
                        "フォーカードのその先へ",
                    }
                },
                .description = {
                    .text = {
                        "Match five or more cards of the same rank.",
                        "Faire disparaître 5 cartes ou plus du même rang.",
                        "同じランクのトランプを5枚以上揃える。",
                    }
                },
                .scoreValue = 20,
            },
            {
                .id = "gameplay-royal-straight",
                .name = {
                    .text = {
                        "Royal Straight",
                        "Quinte Royal",
                        "ロイヤルストレート",
                    }
                },
                .description = {
                    .text = {
                        "Achieve a royal straight with 10, Jack, Queen, King, Ace.",
                        "Réussir une quinte royale avec 10, Valet, Dame, Roi, As.",
                        "10、J、Q、K、Aのロイヤルストレートを達成する。",
                    }
                },
                .scoreValue = 40,
            },
            {
                .id = "gameplay-comeback",
                .name = {
                    .text = {
                        "Reversal",
                        "Retournement de situation",
                        "逆転",
                    }
                },
                .description = {
                    .text = {
                        "Win a match after your board was nearly full.",
                        "Gagner une partie après avoir été au bord de la défaite.",
                        "大ピンチの後、勝利する。",
                    }
                },
                .scoreValue = 30,
            },
            {
                .id = "gameplay-clear",
                .name = {
                    .text = {
                        "All Clear",
                        "As de l'escamotage",
                        "全消し",
                    }
                },
                .description = {
                    .text = {
                        "Clear all cards on the board.",
                        "Faire disparaître toutes les cartes du terrain.",
                        "盤面のすべてのトランプを消す。",
                    }
                },
                .scoreValue = 50,
            },
        }
    }
};
