//
//  story_dove.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#include "story_dove.h"

#include "cpu.h"
#include "instantcpu.h"
#include "patterncpu.h"
#include "randomcpu.h"

const Story kDoveStory = (Story) {
    .character = CharacterCeres,
    .encounters = {
        .count = 5,
        .memory = (Encounter[5]) {
            {
                .opponent = CharacterCaroline,
                .background = BackgroundSchoolCorridor,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((Cpu) {
                    UseRandomCpu(1.0f)
                }),
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Blah blah!",
                                    "Blabla !",
                                    "ぺちゃくちゃ！",
                                    "Blá blá!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Caroline seems to be trying to tell me something.",
                                    "Caroline semble vouloir me de dire quelque chose.",
                                    "キャロラインは何かを伝えようとしているよう。",
                                    "Acho que a Caroline tá tentando me dizer algo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Too bad I can't understand anything she's saying.",
                                    "Dommage que je ne comprenne rien à ce qu'elle me dit.",
                                    "残念だけど、何を言ってるのかさっぱりわからない。",
                                    "Que pena que eu não consigo entender nada que ela fala.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Blah blah Ceres, blah blah blah giving blah blah.",
                                    "Blabla Cérès, bla bla bla bla bla bla bla bla bla mi-graine.",
                                    "えさ、セレス、ぺちゃくちゃ！ぺちゃくちゃ！",
                                    "Blá blá Ceres, blá blá blá dando blá blá.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Hey, did you mention giving me something? Is it snack time?",
                                    "Tiens ça parle de graine. Est-ce que c'est l'heure de manger ?",
                                    "えさの話？食事の時間？",
                                    "Epa, você falou que está me dando alguma coisa? É hora do lanche?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Blah blah blah blah blah blah.",
                                    "Bla bla bla bla bla bla bla.",
                                    "ぺちゃくちゃ。",
                                    "Blá blá blá blá blá blá.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh, those cards look nice. Pretty patterns!",
                                    "Oh, j'aime bien ces cartes. Les motifs sont jolis !",
                                    "きれいなカード見せてくれるけど、えさはどこ？",
                                    "Oh, essas cartas são bonitas. Que padrões lindos!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Is she putting the cards side by side to make them disappear?",
                                    "Elle met des cartes côte à côte pour les faire disparaître ?",
                                    "カードを並べて消しているのかしら？",
                                    "Ela tá colocando as cartas lado a lado pra fazer elas desaparecerem?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Blah blah! Blah blah blah!",
                                    "Bla bla ! Bla bla bla !",
                                    "ぺちゃくちゃ！ぺちゃくちゃ！",
                                    "Blá blá! Blá blá blá!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But? Where are the seeds?",
                                    "Mais? Où sont les graines ?",
                                    "食事の時間じゃなかったの？",
                                    "Mas? Aonde estão as sementes?",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterSapique,
                .background = BackgroundForest,
                .pin = { .x = 176, .y = 100 },
                .actor = (Actor *) &((Cpu) {
                    UseRandomCpu(1.0f),
                    .isPressingDown = true,
                }),
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterCeres,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I wonder where we're going. I hope there's food!",
                                    "Je me demande où nous allons. J'espère qu'il y aura à manger !",
                                    "どこに行くのかな。何か食べ物があればいいな！",
                                    "Aonde será que estamos indo? Espero que tenha comida!",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You shall not pass!",
                                    "Vous ne passerez pas !",
                                    "ここは断じて通さん！",
                                    "Vocês não vão passar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Blahhh! Blah blah blah blah!",
                                    "Blaaa ! Bla bla blabla !",
                                    "ぺちゃくちゃ！ぺちゃくちゃ！",
                                    "Blááá! Blá blá blá blá!",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You shall not pass!",
                                    "Vous ne passerez pas !",
                                    "ここは断じて通さん！",
                                    "Vocês não vão passar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Blah blah blah blah blah, Ceres?",
                                    "Bla bla bla bla bla, Cérès ?",
                                    "ぺちゃくちゃ、セレス？",
                                    "Blá blá blá blá blá, Ceres.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "She's pointing left, let's go!",
                                    "Elle montre la gauche, j'y vais !",
                                    "キャロラインは左を指している、行こう！",
                                    "Ela está apontando para a esquerda, bora!",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "No no no! Nobody passes! You shall not pass on the left side.",
                                    "Non non non ! Personne ne passe ! Vous ne passerez pas par le côté gauche.",
                                    "だめだ！左側でも断じて通さん！",
                                    "Nananinanão! Ninguém passa! Vocês não vão passar pela esquerda.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "How about the right side then?",
                                    "Par la droite alors ?",
                                    "じゃあ、右に行こうか？",
                                    "Que tal o lado direito então?",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You shall not… PASS!",
                                    "Vous ne passerez… PAS !",
                                    "だめだ！ここは絶対に断じて通さん！",
                                    "Vocês não vão… PASSAR!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterChataigne,
                .background = BackgroundForest,
                .pin = { .x = 236, .y = 74 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .sameSuitScore = 1,
                    .rowScore = 1,
                }),
                .dialog = {
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNeed,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "There's a seed under this tree! Bingo!",
                                    "Là par terre sous cet arbre, une graine ! Bingo !",
                                    "この木の下に、種があるよ！よかった！",
                                    "Tem uma semente embaixo dessa árvore! Aí sim!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Snack time!",
                                    "À table !",
                                    "いただきます！",
                                    "Hora do lanche!",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hello there!",
                                    "Bonjour ! ",
                                    "こんにちは！",
                                    "Olá!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What?",
                                    "Comment ?",
                                    "えっ？",
                                    "O quê?",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You're not a seed?",
                                    "Tu n'es pas une graine ?",
                                    "種じゃないの？",
                                    "Você não é uma semente?",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'm a chestnut. Nice to meet you!",
                                    "Je suis une châtaigne. Enchanté !",
                                    "私はクリです。はじめまして！",
                                    "Eu sou uma castanha. Muito prazer!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterBaguetteMagique,
                .background = BackgroundFloweryField,
                .pin = { .x = 256, .y = 156 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .isFastWhenGoodHandIsFound = true,
                    .randomFast = 50,
                    .sameSuitScore = 1,
                    .miss = 40,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterCeres,
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Wow, smells good. Is there bread around here?",
                                    "Quelle bonne odeur, il y a du pain par ici !",
                                    "いい匂いがする。この辺にはパンがあるはず！",
                                    "Uau, que cheiro bom! Será que tem pão por aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterBaguetteMagique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "What a beautiful day.",
                                    "Quelle belle journée.",
                                    "何と美しい日ですね。",
                                    "Mas que dia lindo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Talking bread?",
                                    "Du pain qui parle ?",
                                    "しゃべるパン？",
                                    "Um sanduba falante?",
                                }
                            }
                        },

                        {
                            .character = CharacterBaguetteMagique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Oh no, no, no! I am a living being, not a meal!",
                                    "Ah non, non, non ! Je suis un être vivant, pas un repas !",
                                    "違う違う！私は食べ物じゃないんだから。",
                                    "Epa, nananinanão! Eu sou um ser vivo, não uma refeição!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Just a few crumbs?",
                                    "Juste quelques miettes ?",
                                    "一切れだけ？",
                                    "Nem umas migalhas?",
                                }
                            }
                        },

                        {
                            .character = CharacterBaguetteMagique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Um… sorry, I have to go! Goodbye!",
                                    "Euh… désolé, je dois y aller ! Au revoir !",
                                    "えーと… ごめんなさい、行かないと！またね！",
                                    "Hum… desculpa mas eu tenho que ir! Adeus!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterNone,
                .background = BackgroundFloweryField,
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterCeres,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "He's gone…",
                                    "Il est parti…",
                                    "行ってしまった…",
                                    "O sanduba se foi…",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh? He dropped a lot of bread crumbs! Awesome!",
                                    "Oh ? Il a fait tomber plein de miettes ! Super !",
                                    "あら？パンくずがいっぱい落ちているわ！すごい！",
                                    "Epa! Ele deixou cair várias migalhas! Aí sim!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Bon appetit!",
                                    "Bon appétit !",
                                    "いただたきます！",
                                    "Bora rangar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yummy!",
                                    "C'est bon !",
                                    "おいしい！",
                                    "Nham nham!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Blah blah blah ?",
                                    "Blablabla ?",
                                    "ぺちゃくちゃ?",
                                    "Blá blá blá?",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I still don't know what Caroline is trying to say, but I'm glad we came here.",
                                    "Je ne sais toujours pas ce que veut me dire Caroline mais je suis contente qu'on soit passées par ici.",
                                    "キャロラインが何を言いたいのかまだわからないけど、ここに来てよかった。",
                                    "Eu ainda não faço idéia o que a Caroline tá tentando dizer, mas tô feliz que viemos aqui.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "It's delicious!",
                                    "C'est délicieux !",
                                    "これはすごくおいしい！",
                                    "É uma delícia!",
                                }
                            }
                        },
                    }
                }
            },
        }
    }
};
