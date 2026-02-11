//
//  story_belleplume.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#include "story_belleplume.h"

#include "cpu.h"
#include "instantcpu.h"
#include "randomcpu.h"
#include "patterncpu.h"

const Story kBelleplumeStory = (Story) {
    .character = CharacterBelleplume,
    .encounters = {
        .count = 6,
        .memory = (Encounter[6]) {
            {
                .opponent = CharacterCaroline,
                .background = BackgroundSchoolLibrary,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.2f),
                        .isFastWhenGoodHandIsFound = true,
                        .sameSuitScore = 1,
                    },
                    .matchLimit = 1,
                    .switchToInstantCpuLimit = 24,
                    .backToPatternCpuLimit = 12,
                    .patterns = {
                        .count = 2,
                        .memory = (Pattern[2]) {
                            {
                                .name = "Pattern 1",
                                .size = {
                                    .width = 2,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "B*AABB",
                            },
                            {
                                .name = "Pattern 2",
                                .size = {
                                    .width = 5,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "   B   BA*  BA*",
                            },
                        }
                    }
                }),
                .dialog = {
                    .count = 16,
                    .memory = (Phrase[16]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "This book is perfect, I'll hide the map here.",
                                    "Ce livre est parfait, je vais cacher la carte ici.",
                                    "この本がいい、ここに地図を隠そう。",
                                    "Esse livro é perfeito! Vou esconder o mapa aqui.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Oh, this hat…",
                                    "Tiens, ce chapeau…",
                                    "おや、この帽子は…",
                                    "Ah, esse chapéu…",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "*A few minutes later*",
                                    "*Quelques minutes plus tard*",
                                    "*数分後*",
                                    "*Alguns minutos depois*",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "He took off with my hat!",
                                    "Il est parti avec mon chapeau !",
                                    "私の帽子を持って行ったわ！",
                                    "Ele foi embora com o meu chapéu!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Excuse me ma'am, where are the books about card tricks?",
                                    "Excusez-moi madame, où sont les livres qui parlent des tours de carte ?",
                                    "すみません先生、トランプ手品の本はどこですか？",
                                    "Com licença, senhora, aonde ficam os livros sobre truques de carta?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Second row on the left.",
                                    "Deuxième rangée à gauche.",
                                    "2列目の左にあります。",
                                    "Na segunda prateleira da esquerda.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thank you!",
                                    "Merci !",
                                    "ありがとうございます。",
                                    "Obrigada!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "…",
                                    "…",
                                    "…",
                                    "…",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Something is sticking out of this book. It looks like a map!",
                                    "Tiens, quelque chose dépasse de ce livre. On dirait une carte !",
                                    "あれ、この本に何か挟まっている。地図みたいだ！",
                                    "Tem alguma coisa saindo desse livro. Parece um mapa!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Ceres, look at this map. It talks about a magical hat hidden in a cave. It might help me master my tricks!",
                                    "Cérès, regarde cette carte. Elle parle d'un chapeau magique caché dans une cave. Il pourrait peut-être m'aider à réussir mes tours !",
                                    "セレス、この地図を見て。洞窟に隠れている魔法の帽子の話があるわ。これがあれば手品がうまく出来るかもしれない！",
                                    "Ceres, olha só esse mapa. Tá falando sobre um chapéu mágico escondido numa caverna. Ele pode me ajudar a dominar meus truques!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "*Ahem*",
                                    "*Hum hum*",
                                    "*ゴホンゴホン*",
                                    "*Cof-cof*",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "The cave doesn't seem far, we could go check it out!",
                                    "La cave n'a pas l'air très loin, on pourrait aller voir !",
                                    "洞窟はそんなに遠くないみたい、行ってみましょう！",
                                    "A caverna não parece estar longe, a gente podia ir dar uma olhada!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Miss?",
                                    "Mademoiselle ?",
                                    "キャロラインさん？",
                                    "Senhorita?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yes ma'am?",
                                    "Oui madame ?",
                                    "はい、先生？",
                                    "Sim, senhora?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I'd like to remind you that it's forbidden to speak loudly inside the library.",
                                    "Je vous rappelle qu'il est interdit de parler à haute voix dans l'enceinte de la bibliothèque.",
                                    "大声で話すことは図書館内では禁止です。",
                                    "Gostaria de lembrar que é proibido falar alto dentro da biblioteca.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yes ma'am…",
                                    "Oui madame…",
                                    "はい、先生…",
                                    "Sim, senhora…",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterClown,
                .background = BackgroundVillage,
                .pin = { .x = 102, .y = 116 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.2f),
                        .isFastWhenGoodHandIsFound = true,
                        .sameSuitScore = 1,
                    },
                    .matchLimit = 1,
                    .switchToInstantCpuLimit = 24,
                    .backToPatternCpuLimit = 12,
                    .patterns = {
                        .count = 2,
                        .memory = (Pattern[2]) {
                            {
                                .name = "Pattern 1",
                                .size = {
                                    .width = 4,
                                    .height = 2
                                },
                                .variableCount = 2,
                                .pattern = "  B*BBAA",
                            },
                            {
                                .name = "Pattern 2",
                                .size = {
                                    .width = 6,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "    B*    AA    BB",
                            },
                        }
                    }
                }),
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterCrowd,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "*Bravo! That was awesome!*",
                                    "*Bravo ! C'était magnifique !*",
                                    "*ブラボー！素晴らしかった！*",
                                    "*Bravo! Isso foi incrível!*",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "The audience never gets tired. This is great!",
                                    "Le public ne se lasse pas. C'est super !",
                                    "お客さんはショーを見続けてくれている。すごい！",
                                    "O público não se cansa. Isso é ótimo!",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Maybe I'll try a magic trick…",
                                    "Je vais peut-être tenter un tour de magie…",
                                    "手品を試してみようかな…",
                                    "Talvez eu tente uma mágica…",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Et voilà!",
                                    "Et voilà !",
                                    "ほら！",
                                    "Tchanam!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Not bad, but you have some room for improvement.",
                                    "Pas mal, mais vous avez des progrès à faire.",
                                    "悪くないけど、もっと上達が必要ね。",
                                    "Nada mal, mas você ainda pode melhorar.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "If you want to improve, feel free to visit our magic school.",
                                    "Si vous souhaitez vous améliorer, n'hésitez pas à visiter notre école de magie.",
                                    "もし上達したいなら、私たちの学校にきてくださいね。",
                                    "Se você quiser melhorar, fique à vontade para visitar nossa escola de mágica.",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I like doing small tricks but I don't want to become a magician!",
                                    "Je veux bien faire des petits tours mais pas devenir magicien !",
                                    "簡単な手品だったらOKですが、マジシャンになるつもりはないよ！",
                                    "Eu gosto de fazer truques pequenos mas não quero me tornar um mágico!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterOrelia,
                .background = BackgroundFloweryField,
                .pin = { .x = 290, .y = 60 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.2f),
                    .isFastWhenAboveTarget = true,
                    .sameSuitScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Hello Earika, have you seen Crocoford?",
                                    "Bonjour Orelia, tu n'aurais pas vu Crocoford ?",
                                    "こんにちはミミちゃん、クロコフォードを見かけなかった？",
                                    "Olá Oreia, Você viu o Crocoford?",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yes, I saw him pass by, he was heading towards the mountain.",
                                    "Oui je l'ai vu passer, il se dirigeait vers la montagne.",
                                    "はい、見かけました。山の方に向かって行きました。",
                                    "Sim, ele passou por aqui, estava indo para a montanha.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "And where is my little Melusine?",
                                    "Et où est ma petite Mélusine ?",
                                    "そして、私の可愛いメリュジーヌはどこ？",
                                    "E onde está minha Melusinha?",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "She got tired of looking at my wonderful ears and she…",
                                    "Elle s'est lassée de regarder mes magnifiques oreilles et elle a…",
                                    "彼女は私の美しい耳を見るのに飽きて…",
                                    "Ela cansou de ficar olhando para as minhas orelhas maravilhosas e ela…",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Oops! I promised not to say anything!",
                                    "Oups ! J'ai promis de ne rien dire !",
                                    "あっ！何も言わないと約束したのに！",
                                    "Oops! Eu prometi não contar!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Earika, you promised me you would look after her. Tell me where she's hiding.",
                                    "Orelia, tu m'avais promis que tu veillerais sur elle. Dis-moi où elle se cache.",
                                    "メリちゃんの面倒をちゃんと見てくれると約束してくれたでしょ。どこに隠れているのか教えて。",
                                    "Oreia, você me prometeu que ia cuidar dela. Diga aonde ela está se escondendo.",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It's a secret!",
                                    "C'est un secret !",
                                    "ダメ！秘密です！",
                                    "É segredo!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterPetitChaperonRouge,
                .background = BackgroundForest,
                .pin = { .x = 254, .y = 150 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.2f),
                    .isFastWhenGoodHandIsFound = true,
                    .straightScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Grandmother! I was bringing you a cake and a little pot of butter, just like you like!",
                                    "Mère-grand ! J'étais en train de vous porter une galette et un petit pot de beurre, comme vous aimez !",
                                    "おばあちゃん！ガレットとバターを持ってきました！",
                                    "Vovozinha! Estava te trazendo esses doces, assim como você gosta!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Now Garouge, I'm neither your grandmother nor that old!",
                                    "Voyons Garouge, je ne suis ni ta grand-mère, ni si vieille que ça !",
                                    "ガルージュ、私はあなたのおばあちゃんではないし、そんなに年を取っていないわ！",
                                    "Olha, Lobelinha, eu não sou sua avó e nem tão velha assim!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Can you teach me how to make a flower crown?",
                                    "Est-ce que vous pouvez m'apprendre à faire une couronne de fleurs ?",
                                    "お花の冠の作り方を教えてくれますか？",
                                    "Você pode me ensinar a fazer uma coroa de flores?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Another time, Garouge. I'm in a hurry at the moment.",
                                    "Une autre fois, Garouge. Je suis un peu pressée pour le moment.",
                                    "また今度ね、ガルージュ。今はちょっと急いでいるの。",
                                    "Outra hora, Lobelinha. Estou com pressa agora.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Please!",
                                    "S'il vous plaît !",
                                    "お願いします！",
                                    "Por favor!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionShy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Not now, sorry.",
                                    "Pas maintenant, désolée.",
                                    "今は無理よ、ごめんなさい。",
                                    "Agora não, desculpa.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Please!",
                                    "S'il vous plaît !",
                                    "お願いします！",
                                    "Por favoooor!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterGuide,
                .background = BackgroundForest,
                .pin = { .x = 252, .y = 220 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.2f),
                        .isFastWhenGoodHandIsFound = true,
                        .sameSuitScore = 1,
                    },
                    .matchLimit = 1,
                    .switchToInstantCpuLimit = 24,
                    .backToPatternCpuLimit = 12,
                    .patterns = {
                        .count = 3,
                        .memory = (Pattern[3]) {
                            {
                                .name = "Pattern 1",
                                .size = {
                                    .width = 5,
                                    .height = 3
                                },
                                .variableCount = 4,
                                .pattern = " DCB DCBA*DCBA*",
                            },
                            {
                                .name = "Pattern 2",
                                .size = {
                                    .width = 5,
                                    .height = 3
                                },
                                .variableCount = 4,
                                .pattern = " BCD *ABCD*ABCD",
                            },
                            {
                                .name = "Pattern 3",
                                .size = {
                                    .width = 5,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "  B   *AB  *AB ",
                            },
                        }
                    }
                }),
                .gameSpeed = 3.0f,
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Well, well, who’s hiding behind that tree?",
                                    "Tiens tiens, qui se cache derrière cet arbre ?",
                                    "まあまあ、木の後ろに隠れているのは誰かしら？",
                                    "Ora, ora, quem está se escondendo atrás dessa árvore?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Darn, I've been spotted!",
                                    "Flûte, je suis repéré !",
                                    "しまった、見つかった！",
                                    "Droga, me encontraram!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "This is very suspicious. What’s going on here? You and Crocoford are up to something, aren’t you?",
                                    "Tout ceci est très suspect, que se passe-t-il ici à la fin ? Toi et Crocoford êtes en train de manigancer quelque chose, n'est-ce pas ?",
                                    "なんだかとても怪しいわ、ここで何が起こっているの？あなたとクロコフォードは何かを企んでいるのでしょう？",
                                    "Isso é muito suspeito. O que está acontecendo aqui? Você e o Crocoford estão tramando alguma coisa, não é mesmo?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Not at all, I’m just providing after-sales service.",
                                    "Pas du tout, je ne fais qu'assurer le service après-vente d'une prestation.",
                                    "全く違うよ、私はただアフターサービスを提供しているだけさ。",
                                    "Que nada, só estou prestando serviços de garantia.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m checking if Clovis managed to understand his map. I can’t let a client like him get lost.",
                                    "Je regarde si Clovis a bien réussi à comprendre sa carte. Je ne peux pas laisser un client comme lui se perdre.",
                                    "クロヴィスが地図をちゃんと理解できたかどうかを確認しているんだ。彼のようなお客さんを迷わせるわけにはいかないからね。",
                                    "Estou vendo se Clóvis conseguiu entender o mapa. Não posso deixar um cliente como ele se perder.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Even if he hasn’t paid me…",
                                    "Même s'il ne m'a pas payé…",
                                    "たとえ代金を払っていなくても…",
                                    "Mesmo se ele não me pagou…",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You haven’t convinced me. You’re hiding something.",
                                    "Tu ne m'as pas convaincue. Tu caches quelque chose.",
                                    "納得できないわ。あなたは何かを隠している。",
                                    "Você não me convenceu. Está escondendo alguma coisa.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Where is Crocoford?",
                                    "Où est Crocoford ?",
                                    "クロコフォードはどこ？",
                                    "Aonde está o Crocoford?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "He's…",
                                    "Il est en train de…",
                                    "彼は…",
                                    "Ele…",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Darn again!",
                                    "Re-flûte !",
                                    "しまった！",
                                    "Droga, de novo!",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I won’t say anything!",
                                    "Je ne dirai rien !",
                                    "何も言わない！",
                                    "Não vou dizer nada!",
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
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Well?",
                                    "Alors ?",
                                    "それで？",
                                    "Então?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Okay, okay, I confess.",
                                    "D'accord, d'accord, j'avoue tout.",
                                    "わかった、わかった、全部白状するよ。",
                                    "Tá bom, eu confesso.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Crocoford asked me to help Clovis through the forest.",
                                    "Crocoford m'a demandé d'aider Clovis à traverser la forêt.",
                                    "クロコフォードはクロヴィスが森を抜けるのを手伝ってくれと頼んだんだ。",
                                    "Crocoford me pediu para ajudar o Clóvis pela floresta.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "If you’re looking for him, he should be a bit further, at the foot of the mountain.",
                                    "Si tu le cherches, il doit être un peu plus loin, au pied de la montagne.",
                                    "もし彼を探しているなら、彼はもう少し先の山のふもとにいるはずだ。",
                                    "Se está procurando por ele, ele deve estar um pouco mais adiante, no pé da montanha.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I don’t like this, I’m going!",
                                    "Je n'aime pas ça, j'y vais !",
                                    "嫌な予感がする、行くわよ！",
                                    "Não gosto disso, estou indo lá!",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "And I’m heading back. It wasn’t a good day for business…",
                                    "Et moi je vais rentrer. Ce n'était pas une bonne journée pour les affaires…",
                                    "私は帰るよ。今日は商売日和ではなかったな…",
                                    "E eu vou voltar. Não foi um bom dia para os negócios…",
                                }
                            }
                        },
                    }
                }
            },
        }
    }
};
