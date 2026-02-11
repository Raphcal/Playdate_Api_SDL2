//
//  story_crocoford.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#include "story_crocoford.h"

#include "cpu.h"
#include "randomcpu.h"
#include "instantcpu.h"
#include "patterncpu.h"

const Story kCrocofordStory = (Story) {
    .character = CharacterCrocoford,
    .encounters = {
        .count = 11,
        .memory = (Encounter[11]) {
            {
                .opponent = CharacterCaroline,
                .background = BackgroundSchoolCorridor,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.1f),
                        .isFastWhenGoodHandIsFound = true,
                        .preferSides = true,
                        .sameSuitScore = 1,
                    },
                    .matchLimit = 2,
                    .switchToInstantCpuLimit = 18,
                    .backToPatternCpuLimit = 6,
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
                                    .width = 4,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "  *B  AA  BB",
                            },
                        }
                    }
                }),
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Everything is ready. I found a hat, hid the map in a book.",
                                    "Tout est prêt ! J'ai trouvé un chapeau, caché la carte dans un livre.",
                                    "準備ができた。帽子を見つけ、図書館の本に地図を隠した。",
                                    "Está tudo pronto! Eu encontrei um chapéu e escondi o mapa em um livro.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Now I just need to push Caroline and Clovis to start their adventure.",
                                    "Il ne me reste plus qu'à pousser Caroline et Clovis pour qu'ils partent à l'aventure.",
                                    "あとはキャロラインとクロヴィスを冒険に送り出すだけだ。",
                                    "Agora só preciso dar um empurrãozinho na Caroline e no Clóvis para que comecem sua aventura.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "They are good magicians, but they lack the experience to truly flourish.",
                                    "Ce sont de bons magiciens, mais ils manquent d'expérience pour vraiment s'épanouir.",
                                    "彼らは良いマジシャンだが、本当に成長するためには経験が足りない。",
                                    "Eles são bons mágicos, mas ainda falta experiência para desabrocharem de fato.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I see Caroline, here we go!",
                                    "J'aperçois Caroline, c'est parti !",
                                    "キャロラインが見えた、始めるぞ！",
                                    "Eu vejo a Caroline, aqui vamos nós!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Coo coo!",
                                    "Rrou rrou !",
                                    "ポッポ！",
                                    "Co-có.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I won!",
                                    "J'ai gagné !",
                                    "勝った！",
                                    "Ganhei!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Still wandering the halls instead of studying, Miss Caroline? You should go to the library and review your lessons if you want a chance to pass the exam.",
                                    "Encore à trainer dans les couloirs au lieu de réviser mademoiselle Caroline ? Vous devriez aller réviser à la bibliothèque si vous souhaitez avoir une chance de réussir l'examen.",
                                    "キャロラインさん、また廊下でぶらついているのか？試験に合格したければ、図書館で勉強するべきですよ。",
                                    "Ainda zanzando pelos corredores ao invés de estudar, senhorita Caroline? Você deveria ir à biblioteca e revisar suas lições se quiser ter uma chance para passar no seu exame.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Alright, Mr. Crocoford, I'll go right away.",
                                    "D'accord monsieur Crocoford, j'y vais tout de suite.",
                                    "わかりました、クロコフォード先生。すぐに行きます。",
                                    "Certo, senhor Crocoford, tô indo agora.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Wait, miss. Before I let you go, I'd like to test your current knowledge…",
                                    "Attendez mademoiselle. Avant de vous laisser filer, je voudrais vérifier votre niveau actuel…",
                                    "キャロラインさん、ちょっと待ってください。行く前に、今のあなたの実力を確かめたいのですが…",
                                    "Espere, senhorita. Antes de deixá-la ir, gostaria de testar seu conhecimento…",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterClovis,
                .background = BackgroundSchoolClassroom,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.1f),
                        .isFastWhenGoodHandIsFound = true,
                        .sameSuitScore = 1,
                        .avoidCenter = true,
                    },
                    .matchLimit = 2,
                    .switchToInstantCpuLimit = 18,
                    .backToPatternCpuLimit = 6,
                    .patterns = {
                        .count = 3,
                        .memory = (Pattern[3]) {
                            {
                                .name = "Pattern 1",
                                .size = {
                                    .width = 4,
                                    .height = 3
                                },
                                .variableCount = 3,
                                .pattern = "*BCCAABBCCAA",
                            },
                            {
                                .name = "Pattern 2",
                                .size = {
                                    .width = 2,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "*BAABB",
                            },
                            {
                                .name = "Pattern 3",
                                .size = {
                                    .width = 6,
                                    .height = 3
                                },
                                .variableCount = 2,
                                .pattern = "    *B    AA    BB",
                            },
                        }
                    }
                }),
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Alright, now it’s Clovis’s turn…",
                                    "Bien, maintenant c'est au tour de Clovis…",
                                    "さて、次はクロヴィスの番だ…",
                                    "Certo, agora a vez do Clóvis…",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "He is far too naive to become a good magician. He needs to discover the world.",
                                    "Il est beaucoup trop naïf pour devenir un bon magicien. Il faut qu'il découvre le monde.",
                                    "彼は良いマジシャンになるには素直すぎる。もっと世界を知る必要がある。",
                                    "Ele é muito ingênuo para se tornar um bom mágico. Ele precisa encarar o mundo.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Did you call for me, Professor Crocoford?",
                                    "Vous m'avez appelé professeur Crocoford ?",
                                    "クロコフォード先生、呼びましたか？",
                                    "Você me chamou, professor Crocoford?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Caroline got lost in the woods, I need you to rescue her.",
                                    "Caroline s'est perdue en forêt, il faut que tu ailles la sauver.",
                                    "キャロラインが森で迷子になっている。彼女を助けに行ってほしい。",
                                    "Caroline se perdeu na floresta, eu preciso que você a resgate.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "How do you know she got lost?",
                                    "Comment savez-vous qu'elle s'est perdue ?",
                                    "どうして彼女が迷子になったってわかるんですか？",
                                    "Como você sabe que ela se perdeu?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Good point…",
                                    "Bonne remarque…",
                                    "いい質問だが…",
                                    "Bom ponto…",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But that's a detail! Take this map and go!",
                                    "Mais c'est un détail ! Prend cette carte et vas-y !",
                                    "細かいことは気にしないで、この地図を持って行ってくれ！",
                                    "Mas isso é um detalhe! Pegue esse mapa e vá!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Okay, but isn't it dangerous for me to go alone?",
                                    "D'accord mais, ce n'est pas dangereux pour moi aussi d'aller seul en forêt ?",
                                    "わかりました。でも、一人で行くのは危険ではないですか？",
                                    "Ok, mas não é perigoso ir sozinho?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Wouldn't some training be better?",
                                    "Un petit entraînement ne serait-il pas prudent ?",
                                    "少し訓練した方が良くないですか？",
                                    "Algum tipo de treinamento não seria bom?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "That's an excellent idea, let's get started.",
                                    "C'est une excellente idée, commençons.",
                                    "それは素晴らしい考えだ。始めよう。",
                                    "Uma idéia excelente, vamos começar.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterGuide,
                .background = BackgroundVillage,
                .pin = { .x = 120, .y = 98 },
                .actor = (Actor *) &((PatternCpu) {
                    .super = {
                        UsePatternCpu(0.1f),
                        .isFastWhenGoodHandIsFound = true,
                        .sameSuitScore = 1,
                    },
                    .matchLimit = 2,
                    .switchToInstantCpuLimit = 18,
                    .backToPatternCpuLimit = 6,
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
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Paskey, here you are! I have a client for you!",
                                    "Passepartout, j'ai un client pour toi !",
                                    "パスキー、依頼人がいる！",
                                    "Passaqui, te achei! Tenho um cliente para você!",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionMeh,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Not so fast, Crocoford, the last service I did for you cost me a lot.",
                                    "Pas si vite Crocoford, le dernier service que je t'ai rendu m'a coûté cher.",
                                    "待ってくれ、クロコフォード。最後に君のためにした仕事は高くついた。",
                                    "Peraí, Crocoford, o último serviço que eu fiz pra você me custou caro.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You don’t have to worry, this one is a good client.",
                                    "Tu n'as pas à t'inquiéter, celui là est un bon client.",
                                    "心配はいらない、今回は良い依頼人だ。",
                                    "Não se preocupe, esse é um bom cliente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I’ll even advance you a gold coin for your services.",
                                    "Je t'avance même une pièce d'or pour tes services.",
                                    "金貨1枚を前払いしよう。",
                                    "Vou te antecipar uma moeda de ouro como pagamento pelos seus serviços.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionMeh,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Where’s the catch?",
                                    "Où est le piège ?",
                                    "どんな裏があるんだ？",
                                    "Qual é a pegadinha?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "There’s no catch! He's a student of mine.",
                                    "Il n'y a pas de piège ! C'est un élève à moi. ",
                                    "裏なんてない！彼は私の生徒だ。",
                                    "Não tem pegadinha! Ele é um aluno meu.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "However, you might need to keep an eye on him. Just in case…",
                                    "Par contre, il faudrait que tu le surveilles un peu. Au cas où…",
                                    "ただ、彼を少し見守っていて欲しい。万が一のために…",
                                    "Porém, talvez você deva ficar de olho nele. Por via das dúvidas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "He's good at sleight of hand, but he's a bit lacking in common sense.",
                                    "Il est bon en prestidigitation mais il manque un peu de jugeote.",
                                    "彼は良いマジシャンだが、少し判断力に欠けているんだ。",
                                    "Ele é bom na prestidigitação, mas falta bom senso.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Alright, but only if you win against me!",
                                    "Bon d'accord, mais seulement si tu gagnes contre moi !",
                                    "よし、わかった。でも勝負に勝ったらだ！",
                                    "Certo, mas só se você me vencer!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "With pleasure!",
                                    "Bien volontier !",
                                    "喜んで！",
                                    "Com prazer!",
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
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isAlwaysFast = true,
                    .straightScore = 8,
                    .rowScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You shall not pass!",
                                    "Vous ne passerez pas !",
                                    "ここは断じて通さん！",
                                    "Você não vai passar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Of course, please excuse me. After you…",
                                    "Bien sûr, veuillez m'excuser. Après vous…",
                                    "もちろん、失礼しました。どうぞお先に…",
                                    "Claro, com toda licença, por favor. Depois do senhor…",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I will do no such thing, you go first.",
                                    "Je n'en ferai rien, vous d'abord.",
                                    "そんなことはさせない、君が先だ。",
                                    "Não seja por isso, o senhor primeiro.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You’re too kind. I'll go then.",
                                    "Vous êtes trop aimable. Je passe alors.",
                                    "あなたはとても親切ですね。では、通らせてもらいます。",
                                    "Você é muito gentil. Eu irei então.",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Please, go ahead.",
                                    "Je vous en prie.",
                                    "どうぞお通りください。",
                                    "Por favor, eu insisto.",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
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
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Wait?",
                                    "Mais ?",
                                    "待って？",
                                    "Espera?!",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You almost had me! You shall not pass!",
                                    "Vous avez failli m'avoir ! Vous ne passerez pas !",
                                    "危うく騙されるところだった！ここは断じて通さん！",
                                    "Você quase me pegou! Você não vai passar!",
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
                    UseInstantCpu(0.1f),
                    .isFastWhenAboveTarget = true,
                    .preferSides = true,
                    .sameSuitScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Have you come to see Melusine, Professor?",
                                    "Vous êtes venu voir Mélusine, professeur ?",
                                    "先生、メリュジーヌに会いに来ましたか？",
                                    "Você veio ver a Melusina, professor?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "No, I have some business further in the forest.",
                                    "Non, j'ai une affaire un peu plus loin dans la forêt.",
                                    "いや、私は森の奥に用事があるんだ。",
                                    "Não, mas eu tenho outros negócios a resolver mais adiante na floresta.",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "She has studied her magic tricks a lot and she’s eager to show you her new trick.",
                                    "Elle a beaucoup révisé ses tours de magie et elle est impatiente de vous montrer son nouveau tour.",
                                    "彼女は手品の練習をたくさんしていて、新しい手品を見せたがっています。",
                                    "Ela estudou bastante suas mágicas e mal pode esperar para te mostrar o novo truque dela.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "That will be a nice surprise for tonight!",
                                    "Ça fera une belle surprise pour ce soir !",
                                    "それは今夜の素晴らしいサプライズになるだろう！",
                                    "Isso será uma boa surpresa para hoje à noite!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Tonight? Why not watch it now?",
                                    "Ce soir ? Pourquoi ne pas regarder maintenant ?",
                                    "今夜ですか？なぜ今見ないのですか？",
                                    "Hoje à noite? Por que não ver agora?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I’m really in a hurry.",
                                    "Je suis vraiment pressé.",
                                    "本当に急いでいるんだ。",
                                    "Eu estou com muita pressa.",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionAngry,
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
                            .character = CharacterOrelia,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "She's your daughter. You should spend a little more time with her!",
                                    "C'est votre fille, professeur. Vous devriez passer un peu plus de temps avec elle !",
                                    "あなたの娘ですよ、先生。もう少し一緒にいてあげてください！",
                                    "Ela é sua filha. Você deveria passar mais tempo com ela!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'm outraged!",
                                    "Je suis outrée !",
                                    "ガッカリ！",
                                    "Mas que raiva!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterPetitChaperonRouge,
                .background = BackgroundFloweryField,
                .pin = { .x = 254, .y = 150 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .straightScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 12,
                    .memory = (Phrase[12]) {
                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "La la la ♪",
                                    "La la la ♪",
                                    "ラララ♪",
                                    "Lá lá lá ♪",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "*Boom!*",
                                    "* Boum ! *",
                                    "*ドン！*",
                                    "*Bum!*",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "La la ♪",
                                    "La la ♪",
                                    "ララ♪",
                                    "Lá lá ♪",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "*Bam!*",
                                    "* Badaboum ! *",
                                    "*バタン！*",
                                    "*Bam!*",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hello!",
                                    "Bonjour !",
                                    "こんにちは！",
                                    "Olá!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Is it you making all this noise?",
                                    "C'est vous qui faites tout ce bruit ?",
                                    "この騒音はあなたが？",
                                    "É você que está fazendo todo esse barulho?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "No, but…",
                                    "Non mais…",
                                    "いや、でも…",
                                    "Não, mas…",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "*Boom! Boom!*",
                                    "* Boum ! Boum !*",
                                    "*ドン！ドン！*",
                                    "*Bum! Bum!*",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I have an idea…",
                                    "j'ai une idée…",
                                    "考えがある…",
                                    "Eu tenho uma idéia…",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "*Crash!*",
                                    "* Braoum !*",
                                    "*ドーン！*",
                                    "*Xablau!*",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "…about the culprit.",
                                    "du responsable.",
                                    "犯人の。",
                                    "…de quem está.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "This is unbearable!",
                                    "C'est insupportable !",
                                    "もう我慢できない！",
                                    "Assim não dá!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterElJoker,
                .background = BackgroundRocks,
                .pin = { .x = 276, .y = 190 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenAboveTarget = true,
                    .straightScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Disappear!",
                                    "Desaparece !",
                                    "消えろ！",
                                    "Desapareça!",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "Boom!",
                                    "Boum !",
                                    "ドーン！",
                                    "Bum!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Sorry to interrupt you, El Joker, but you are a bit too noisy.",
                                    "Désolé de t'interrompre El Joker mais tu es un peu trop bruyant.",
                                    "邪魔して悪いが、エルジョーカー、少し騒がしすぎる。",
                                    "Desculpe te interromper, El Joker, mas você está fazendo muito barulho.",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Of course! Explosions, loud noises, they excite the audience!",
                                    "Bien sûr ! Les explosions, les bruits forts, ça fait vibrer le public !",
                                    "もちろん！爆発音、大きな音、それが観客を興奮させるのさ！",
                                    "Mas é claro! Explosões, barulhos, isso anima a platéia!",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m practicing for our next duel, amigo, you won’t beat me next time!",
                                    "Je m'entraîne pour notre prochain duel amigo, tu ne m'auras pas la prochaine fois !",
                                    "次の対決のために練習してるんだ、アミーゴ、次は負けないぞ！",
                                    "Estou praticando para o nosso próximo duelo, compañero, você não vai me vencer!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "If I beat you now, can you practice more quietly?",
                                    "Si je te bats maintenant, est-ce que tu pourras t'entraîner plus silencieusement ?",
                                    "今君に勝ったら、もっと静かに練習してくれるか？",
                                    "Se eu te vencer agora, você pode praticar mais baixo?",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I can’t promise that, but I always accept challenges! Vamos!",
                                    "Je ne peux pas te le promettre mais j'accepte toujours les défis ! Vamos !",
                                    "それは約束できないが、挑戦はいつでも受ける！いくぞ！",
                                    "Não posso prometer isso, mas sempre aceito um desafio! Vamonos!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterCheval,
                .background = BackgroundForest,
                .pin = { .x = 300, .y = 230 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .preferSides = true,
                    .straightScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I’ll hide here while waiting for Caroline and Clovis.",
                                    "Je vais me cacher ici en attendant Caroline et Clovis.",
                                    "キャロラインとクロヴィスを待つ間、ここに隠れよう。",
                                    "Vou me esconder aqui enquanto espero pela Caroline e o Clóvis.",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Excuse me, sir, but this is my hiding spot.",
                                    "Excusez-moi monsieur mais c'est ma cachette ici.",
                                    "すみませんが、ここは私の隠れ場所です。",
                                    "Com licença senhor, mas esse é o meu esconderijo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Your hiding spot? Why are you hiding?",
                                    "Votre cachette ? Pourquoi vous cachez vous ?",
                                    "隠れ場所？なぜ隠れているんだ？",
                                    "Seu esconderijo? Porque está se escondendo?",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I ran away from the prince.",
                                    "Je me suis enfuit de chez le prince.",
                                    "王子から逃げ出したのです。",
                                    "Eu fugi do príncipe.",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Too little freedom, only carrots for meals, I couldn’t take it anymore!",
                                    "Trop peu de liberté, que des carottes aux repas, je n'en pouvais plus !",
                                    "自由がなく、食事もニンジンばかり、もう我慢できませんでした！",
                                    "Pouca liberdade, apenas cenouras de refeição, eu não aguentava mais!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Carrots are full of vitamin A. It's good for your eyesight. You should go back!",
                                    "Les carottes sont riches en vitamines A. C'est important pour la vue. Vous devriez y retourner !",
                                    "ニンジンにはビタミンAがたっぷり。目にとって重要だ。戻るべきだ！",
                                    "Cenouras são cheias de vitamina A. Faz bem para a vista. Você deveria voltar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Never!",
                                    "Jamais !",
                                    "絶対に戻らない！",
                                    "Nem a pau, Juvenal!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterMelusine,
                .background = BackgroundCaveEntrance,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenAboveTarget = true,
                    .sameKindScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Great! I passed the exam!",
                                    "Super ! J'ai réussi l'examen !",
                                    "すごい！試験に合格した！",
                                    "Que demais! Passei na prova!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Me too, this is awesome!",
                                    "Moi aussi, c'est génial !",
                                    "僕もだ、素晴らしい！",
                                    "Eu também, isso é incrível!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Both of you, please be quiet.",
                                    "S'il vous plaît, faites un peu moins de bruit tous les deux.",
                                    "二人とも、静かにしてくれ。",
                                    "Vocês dois, silêncio por favor.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "We still don’t know who attacked you in the cave.",
                                    "Nous ne savons toujours pas qui vous a attaqué dans la grotte.",
                                    "洞窟で誰が君たちを襲ったのか、まだわからない。",
                                    "Ainda não sabemos quem os atacou na caverna.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Dad?",
                                    "Papa ?",
                                    "パパ？",
                                    "Papai?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Dad!",
                                    "Papa !",
                                    "パパ！",
                                    "Papai!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Melusine? What are you doing here?",
                                    "Mélusine ? Mais qu'est-ce que tu fais là ?",
                                    "メリュジーヌ？ここで何をしている？",
                                    "Melusina? O que você está fazendo aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You should be with Earika. Did you run away? You know you shouldn’t do that.",
                                    "Tu devrais être avec Orelia. Tu as fait une fugue ? Tu sais bien qu'il ne faut pas faire ce genre de choses.",
                                    "なぜミミちゃんのところにいない？逃げ出したのか？そんなことをしてはいけないってわかっているだろう。",
                                    "Você deveria estar com a Oreia. Você fugiu? Sabe que não deveria fazer isso.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I followed you, Dad. You didn’t let me show you my new trick when you stopped by Earika’s!",
                                    "Je t'ai suivi papa. Tu ne m'as pas laissé te montrer mon nouveau tour de magie quand tu es passé chez Orelia !",
                                    "パパについてきたの。ミミさんのところに寄った時に、私の新しい手品を見てくれなかったから！",
                                    "Eu te segui papai. Você não me deixou te mostrar meu novo truque quando passou pela creche da Oreia!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Alright, you can show me now.",
                                    "D'accord, tu peux me montrer ça maintenant.",
                                    "わかった、今見せてくれ。",
                                    "Muito bem, pode me mostrar agora.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But quickly, I sense danger approaching…",
                                    "Mais rapidement, je sens qu'un danger approche…",
                                    "でも早く、危険が近づいている気がする…",
                                    "Mas rápido, Eu sinto perigo se aproximando…",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterBelleplume,
                .background = BackgroundCaveEntrance,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenAboveTarget = true,
                    .preferSides = true,
                    .straightScore = 8,
                    .rowScore = 1,
                }),
                .gameSpeed = 3.0f,
                .dialog = {
                    .count = 12,
                    .memory = (Phrase[12]) {
                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "So, how was it dad?",
                                    "Alors, comment c'était papa ?",
                                    "どうだったパパ？",
                                    "Então, o que achou papai?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You have made great progress! Congratulations!",
                                    "Tu as beaucoup progressé ! Félicitations !",
                                    "すごく上達したね！おめでとう！",
                                    "Você evoluiu bastante! Parabéns!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Now, if you’ll excuse me. I must leave before she arrives…",
                                    "Maintenant, si vous voulez bien m'excuser. Il faut que je parte avant qu'elle n'arrive…",
                                    "さて、失礼するよ。彼女が来る前に出発しなければ…",
                                    "Agora, se me dão licença. Eu preciso ir embora antes que ela chegue…",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "She? Who are you talking about?",
                                    "Elle ? De qui parlez-vous professeur ?",
                                    "彼女？誰のことですか？",
                                    "Ela? De quem você tá falando?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Honey? I know you're here!",
                                    "Chéri ? Je sais que tu es là !",
                                    "あなた、ここにいるのはわかってるわよ！",
                                    "Querido? Eu sei que você está aqui!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Too late!",
                                    "Trop tard !",
                                    "遅かったか！",
                                    "Tarde demais!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You won’t get away with this!",
                                    "Tu ne t'en tireras pas comme ça !",
                                    "逃げられると思わないで！",
                                    "Você não vai se safar dessa!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Melusine ran away from Earika because of you!",
                                    "Mélusine s'est enfuie de chez Orelia par ta faute !",
                                    "ミミさんのところからメリちゃんが逃げ出したのはあなたのせいよ！",
                                    "Melusina fugiu da creche da Oreia por sua causa!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "She could have gotten lost or captured!",
                                    "Elle aurait pu se perdre ou se faire capturer !",
                                    "迷子になったり誰かに捕まえられたりするかもしれなかったのよ！",
                                    "Ela poderia ter se perdido ou raptada!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "And also, you took my hat without asking!",
                                    "Et aussi, tu as pris mon chapeau sans me le demander !",
                                    "それに、私の帽子を勝手に持っていったわね！",
                                    "E ainda por cima, você pegou o meu chapéu sem permissão!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Sorry, dear, I did it for the students…",
                                    "Désolé chérie, j'ai fait ça pour mes élèves…",
                                    "ごめん、生徒のためにやったんだ…",
                                    "Desculpa, meu amor, eu fiz para os alunos…",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You should have found another way!",
                                    "Tu n'avais qu'à trouver un autre moyen !",
                                    "他の方法を見つけるべきだったわね！",
                                    "Você deveria ter pensando em outra coisa!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterNone,
                .background = BackgroundCaveEntrance,
                .dialog = {
                    .count = 18,
                    .memory = (Phrase[18]) {
                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Melusine isn't hurt so I forgive you this time.",
                                    "Mélusine n'a rien donc je te pardonne pour cette fois.",
                                    "メリュジーヌは無事だったので、今回は許してあげる。",
                                    "Melusina não se machucou então eu te perdoo dessa vez.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "But you should have talked to me before you planned all this.",
                                    "Mais tu aurais dû m'en parler avant de prévoir tout ça.",
                                    "でも、こんなことを計画する前に相談してね。",
                                    "Mas você deveria ter me consultado antes de planejar tudo isso.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You're right, I'm sorry.",
                                    "Tu as raison, excuse moi.",
                                    "すみません、おっしゃる通り。",
                                    "Tem razão, me desculpe.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Congratulations Caroline and Clovis. You are excellent magicians!",
                                    "Félicitations Caroline et Clovis. Vous êtes d'excellents magiciens !",
                                    "おめでとう、キャロラインとクロヴィス。あなたたちは優秀なマジシャンです！",
                                    "Parabéns Caroline e Clóvis. Vocês são mágicos de primeira!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Exactly, you've made huge progress! Congratulations!",
                                    "Exactement, vous avez fait d'énormes progrès ! Félicitations !",
                                    "素晴らしかった！おめでとう！",
                                    "Exatamente, vocês evoluiram muito! Parabéns!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thank you teacher!",
                                    "Merci professeur !",
                                    "先生、ありがとうございます！",
                                    "Valeu prof!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thank you!",
                                    "Merci !",
                                    "ありがとう！",
                                    "Obrigado!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "And you too Melusine, your new card tricks are great.",
                                    "Et toi aussi Mélusine, tes nouveaux tours sont formidables.",
                                    "メリュジーヌも、新しいトランプ手品が素晴らしい。",
                                    "E você também Melusina, seu novo truque de cartas é fantástico.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yay! Thanks dad!",
                                    "Youpi ! Merci papa !",
                                    "やったー！ありがとうパパ！",
                                    "Eba! Obrigada papai!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Here, Mum, your hat! Dad put it in this cave.",
                                    "Tiens maman, ton chapeau ! Papa l'avait posé dans la grotte.",
                                    "ママ、帽子だよ！パパがこの洞窟に入れたの。",
                                    "Aqui, mamãe, seu chapéu! Papai colocou ele nessa caverna.",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thanks Melusine!",
                                    "Merci Mélusine !",
                                    "ありがとう。",
                                    "Muito obrigada Melusina!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
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
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Honey?",
                                    "Chérie ?",
                                    "ハニー？",
                                    "Querido?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes… ?",
                                    "Oui… ?",
                                    "はい…？",
                                    "Sim… ?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Look what happened to my hat! It's all ruined now!",
                                    "Regarde ce qui est arrivé à mon chapeau ! Il est tout abimé maintenant !",
                                    "帽子がどうなったか見て！もう台無しだわ！",
                                    "Olha o que aconteceu com o meu chapéu! Está todo arruinado!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Sorry everyone, but I have some overdue papers to grade! I have to go!",
                                    "Désolé tout le monde mais j'ai des copies à corriger en retard ! Je vous laisse !",
                                    "みんなすまない。採点をしなければならないものがいくつかあって、もう行かないといけない。",
                                    "Com licença a todos, mas eu estou atrasado para corrigir alguns trabalhos! Tenho que ir!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You’re not getting away that easily!",
                                    "Tu ne t'en tireras pas comme ça !",
                                    "そんな理由で逃げられないわよ！",
                                    "Você não vai escapar assim tão fácil!",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "The end!",
                                    "Fin !",
                                    "終わり！",
                                    "O fim!",
                                }
                            }
                        },
                    }
                }
            },
        }
    }
};
