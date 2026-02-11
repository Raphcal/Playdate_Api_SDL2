//
//  story_clovis.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 30/03/2024.
//

#include "story_clovis.h"

#include "cpu.h"
#include "instantcpu.h"
#include "randomcpu.h"

const Story kClovisStory = (Story) {
    .character = CharacterClovis,
    .encounters = {
        .count = 11,
        .memory = (Encounter[11]) {
            {
                .opponent = CharacterCrocoford,
                .background = BackgroundSchoolClassroom,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                    .rowScore = 2,
                }),
                .dialog = {
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Caroline is lost in the woods, I need you to rescue her.",
                                    "Caroline s'est perdue en forêt, il faut que tu ailles la sauver.",
                                    "キャロラインが森で迷子になっている。彼女を助けに行ってほしい。",
                                    "Caroline se perdeu na floresta, eu preciso que você a resgate.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .side = SideRight,
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
                            .side = SideLeft,
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
                            .side = SideLeft,
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
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "That's an excellent idea, let's get started.",
                                    "C'est une excellente idée, commençons.",
                                    "それは素晴らしい考えだ。始めよう。",
                                    "Uma ideia excelente, vamos começar.",
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
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameSuitScore = 1,
                }),
                .dialog = {
                    .count = 13,
                    .memory = (Phrase[13]) {
                        {
                            .character = CharacterCrowd,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "*Bravo! What a show! Encore!*",
                                    "*Bravo ! Quel spectacle ! Encore !*",
                                    "*ブラボー！素晴らしいショーだ！アンコール！*",
                                    "*Bravo! Que show! Bis!*",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "What a crowd! This duel with that magician girl has really revived my career!",
                                    "Quelle foule ! Ce duel avec la magicienne a vraiment relancé ma carrière !",
                                    "なんてことだ！あのマジシャンの女の子との対決でお客さんが戻ってきた！",
                                    "Mas que público! Esse duelo contra a garota mágica deu uma revivida na minha carreira!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Excuse me, do you have a minute?",
                                    "Pardon de vous interrompre…",
                                    "すみません、ちょっとお時間ありますか？",
                                    "Com licença, você tem um minutinho?",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Another magician? What's going on today?",
                                    "Encore un magicien ? Que se passe-t-il aujourd'hui ?",
                                    "またマジシャンか？今日はどうなってるんだ？",
                                    "Outro mágico? O quê está acontecendo hoje?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I heard you mention a magician girl. By any chance, was she accompanied by a dove?",
                                    "Je vous ai entendu parler d'une magicienne. Est-ce que par hasard, elle était accompagnée d'une colombe ?",
                                    "あなたがマジシャンの女の子について話しているのを聞きました。もしかして、彼女は鳩と一緒でしたか？",
                                    "Eu ouvi você mencionar uma garota mágica. Ela estaria acompanhada de uma pomba?",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yes, exactly!",
                                    "Oui, tout à fait !",
                                    "そうだ、その通りだ！",
                                    "Sim, exatamente!",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "But tell me, you're wearing a ringmaster's outfit, you seem to like the circus. We could perform together. Do you know how to juggle?",
                                    "Mais dis-moi, tu portes une tenue de Monsieur Loyal, tu as l'air d'aimer le cirque. On pourrait faire un spectacle ensemble. Est-ce que tu sais jongler ?",
                                    "でも君はサーカスの団長の衣装を着ているね、サーカスが好きなんだろう。我々と一緒にショーに出るのはどうだ？ジャグリングはできるかい？",
                                    "Mas me diga, você está usando uma roupa de mestre do picadeiro, você parece gostar de circo. Poderíamos nos apresentar juntos. Você sabe como fazer malabarismo?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Not really…",
                                    "Non pas spécialement…",
                                    "あまり得意ではありません…",
                                    "Nem um pouco…",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Then you'll learn! Take these balls. Everyone's watching you!",
                                    "Tu vas pouvoir apprendre alors ! Prends ces balles. Tout le monde te regarde !",
                                    "じゃあ、練習すればいいさ！このボールを持って。みんな君を見ているぞ！",
                                    "Então você vai aprender! Pegue essas bolas. Está todo mundo te olhando!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "*Attempts to juggle.*",
                                    "*Essaye de jongler.*",
                                    "*ジャグリングをする。*",
                                    "*Tenta o malabarismo.*",
                                }
                            }
                        },

                        {
                            .character = CharacterCrowd,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "*Laughter*",
                                    "*Rires*",
                                    "*笑い声*",
                                    "*Risos*",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Indeed, you have some progress to make…",
                                    "Effectivement, tu as des progrès à faire…",
                                    "確かに、君はまだ練習が必要だな…",
                                    "De fato, você ainda tem muito que aprender…",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Let's have a duel instead!",
                                    "Faisons un duel de magie dans ce cas !",
                                    "代わりに対決をしよう！",
                                    "Então vamos duelar!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterGuide,
                .background = BackgroundVillage,
                .pin = { .x = 144, .y = 96 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                    .sameSuitScore = 2,
                    .straightScore = 3,
                }),
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "And now, which way should I go?",
                                    "Et maintenant, quelle direction prendre ?",
                                    "さて、どちらの方向に行けばいいのだろう？",
                                    "E agora, que caminho seguir?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Greetings, friend! I'm a guide, and you seem to need my services. Need help reading that map?",
                                    "Salutations, l’ami ! Je suis guide et tu sembles avoir besoin de mes services. Besoin d’un coup de main pour lire cette carte ?",
                                    "こんにちは、友よ！私は道案内人だ。君には私の案内が必要そうだね。地図を読むのを手伝おうか？",
                                    "Olá amigo! Eu sou um guia e você parece estar precisando dos meus serviços. Quer ajuda para ler esse mapa?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes, I can't make any sense of it!",
                                    "Oui, je n'y comprends rien !",
                                    "はい、全然わかりません！",
                                    "Sim, eu não consigo entender ele!",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It's simple. Go straight, then left, then around that, continue for 200 steps, then right and then…",
                                    "C'est tout simple. Tu prends tout droit, puis à gauche, ensuite tu contournes ça, tu continues pendant 200 pas, ensuite à c'est à droite et après…",
                                    "簡単だ。まっすぐ進んで、左に曲がって、200歩進んで、右に曲がって…",
                                    "É simples. Vá reto, e então a esquerda, depois em volta disso, continue por 200 passos, então a direita, e ai…",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionKirakira,
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
                            .character = CharacterGuide,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Or I could go with you and show you the way for a modest fee of 10 gold coins.",
                                    "Ou sinon je peux t'accompagner et te montrer le chemin pour la modique somme de 10 pièces d'or.",
                                    "それとも、一緒に行って道を教えるよ。料金は金貨10枚だ。",
                                    "Ou eu poderia ir contigo e te mostrar o caminho pela modesta quantia de 10 moedas de ouro. ",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But that's a fortune!",
                                    "Mais c'est une fortune !",
                                    "でも、それは高すぎる！",
                                    "Mas isso é uma fortuna!",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "How about a little bet? A duel, if you win, I'll give you a friendly price.",
                                    "Que dirais-tu d’un petit pari ? Un duel, si tu gagnes, je te fais un prix d’ami.",
                                    "小さな賭けをしないか？対決で君が勝ったら特別価格で教えてあげる。",
                                    "Que tal uma aposta? Um duelo, se você vencer eu te dou um descontinho amigo.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Deal!",
                                    "Marché conclu !",
                                    "取引成立！",
                                    "Fechado!",
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
                    UseInstantCpu(0.5f),
                    .isAlwaysFast = true,
                    .straightScore = 1,
                    .rowScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
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
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Too bad! How to go through?",
                                    "Flûte ! Comment faire ?",
                                    "困ったな！どうやって通るんだ？",
                                    "Que droga! Como vamos passar?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "We have a pass.",
                                    "Nous avons un laissez-passer.",
                                    "通行証がある。",
                                    "Eu tenho um passe.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Great!",
                                    "Super !",
                                    "素晴らしい！",
                                    "Legal!",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "A pass? Let me see that.",
                                    "Un laissez-passer ? Faites-moi voir ça.",
                                    "通行証だって？見せてみろ。",
                                    "Um passe? Deixe me ver.",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Everything seems in order, you may pass.",
                                    "Tout est en règle, vous pouvez passer.",
                                    "すべて問題ないようだ、通ってよし。",
                                    "Parece que está tudo em ordem, podem passar.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thank you.",
                                    "Merci.",
                                    "ありがとう。",
                                    "Obrigado.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "How did you get that pass?",
                                    "Comment avez-vous obtenu ce laissez-passer ?",
                                    "どうやってその通行証を手に入れたんだ？",
                                    "Como você conseguiu esse passe?",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Keep it between us, but it's a fake.",
                                    "Ça reste entre nous, mais c'est un faux.",
                                    "ここだけの話だが、それは偽物だ。",
                                    "Só aqui entre nós, ele é falso.",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I heard everything!",
                                    "J'ai tout entendu !",
                                    "全部聞こえてるぞ！",
                                    "Eu ouvi isso!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterPrince,
                .background = BackgroundForest,
                .pin = { .x = 294, .y = 112 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isAlwaysFast = true,
                    .sameSuitScore = 1,
                    .straightScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "From here on, it's straight ahead. You can't miss it.",
                                    "À partir de maintenant, c'est tout droit. Tu ne peux pas te tromper.",
                                    "ここからはまっすぐだ。もう間違えないだろう。",
                                    "Daqui pra frente é só seguir reto. Não tem como se perder.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Thank you for everything.",
                                    "Merci pour tout.",
                                    "ありがとう！",
                                    "Obrigado por tudo.",
                                }
                            }
                        },

                        {
                            .character = CharacterGuide,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "That will be a total of 9 gold coins, friendly price.",
                                    "Ça te fera donc un total de 9 pièces d'or, prix d'ami.",
                                    "料金は金貨9枚、特別価格だ。",
                                    "Isso vai custar um total de 9 moedas de ouro, descontinho amigo.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "It's more than I thought but thanks for the discount I guess…",
                                    "C'est plus que ce que je pensais, mais merci pour la réduction…",
                                    "思ったより高いけど、値引きしてくれてありがとう…",
                                    "É mais do que eu pensava, mas obrigado pelo desconto eu acho…",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionNeed,
                            .side = SideRight,
                            .effect = EffectShake,
                            .line = {
                                .text = {
                                    "Hold it!",
                                    "Un instant !",
                                    "待て！",
                                    "Espera aí!",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "9 gold coins? I won't let a scammer swindle a child out of such a fortune. En garde!",
                                    "9 pièces d'or ? Je ne laisserai pas un escroc soutirer une telle fortune à un bambin. En garde !",
                                    "金貨9枚だって？子供からそんな大金を騙し取ろうなんて許さない。構えろ！",
                                    "9 moedas de ouro? Não vou deixar um charlatão se aproveitar de uma criancinha. En garde!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "A child? But I'm not a baby!",
                                    "Un bambin ? Mais je ne suis pas un bébé !",
                                    "子供？でも僕は小さな子供じゃない！",
                                    "Uma criancinha? Mas eu já sou crescido!",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'm trying to help you, let me handle this!",
                                    "Je cherche à vous aider, laissez-moi faire !",
                                    "君を助けようとしているんだ、私に任せて！",
                                    "Estou tentando te ajudar, deixa comigo!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Apologize first!",
                                    "Excusez-vous d'abord !",
                                    "まず謝ってください！",
                                    "Peça desculpas primeiro!",
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
                    UseInstantCpu(0.5f),
                    .isAlwaysFast = true,
                    .preferSides = true,
                    .sameSuitScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "La la la ♪",
                                    "La la la ♪",
                                    "ラララ ♪",
                                    "Lá lá lá ♪",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What are you doing in the middle of the forest?",
                                    "Que fais-tu en plein milieu de la forêt ?",
                                    "森の中で何をしているんですか？",
                                    "O que você está fazendo pela estrada a fora?",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'm visiting my grandmother to bring her a cake and a little pot of butter.",
                                    "Je rends visite à ma mère-grand pour lui apporter une galette et un petit pot de beurre.",
                                    "祖母のところにガレットとバターを届けに行くの。",
                                    "Vou levar esses doces para a vovozinha.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What big arms you have!",
                                    "Que vous avez de grands bras !",
                                    "なんて大きな腕なんだ！",
                                    "Que braços grandes você tem!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "All the better to carry my basket, sir.",
                                    "C'est pour mieux porter mon panier, monsieur.",
                                    "このかごを運ぶためです。",
                                    "É para carregar minha cesta melhor, moço.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What big ears you have!",
                                    "Que vous avez de grandes oreilles !",
                                    "なんて大きな耳なんだ！",
                                    "Que orelhas grandes você tem!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "All the better to hear the birds singing, sir.",
                                    "C'est pour mieux entendre les oiseaux chanter, monsieur.",
                                    "鳥のさえずりをよりよく聞くためです。",
                                    "É para ouvir os pássaros cantando melhor, moço.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What big teeth you have!",
                                    "Que vous avez de grandes dents !",
                                    "なんて大きな歯なんだ！",
                                    "Que dentes grandes você tem!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
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
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'd rather not talk about it, it makes me feel bad.",
                                    "Je préfère ne pas en parler, ça me complexe un peu.",
                                    "それについては話したくない、気分が悪くなるから。",
                                    "Não gosto de falar sobre isso, eles fazem me sentir mal.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterChataigne,
                .background = BackgroundForest,
                .pin = { .x = 226, .y = 180 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .sameKindScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Paskey told me to go straight, but there are only trees here.",
                                    "Passepartout m'a dit d'aller tout droit mais il n'y a que des arbres par ici.",
                                    "パスキーはまっすぐ行けと言ったが、ここには木しかない。",
                                    "Passaqui me disse para ir reto, mas só tem árvores aqui.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Maybe I should have followed the path?",
                                    "J'aurais peut-être dû suivre le sentier ?",
                                    "道のある方に行くべきだったかな？",
                                    "Talvez eu devesse ter seguido a trilha?",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Ouch!",
                                    "Aïe !",
                                    "痛い！",
                                    "Ai!",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Watch where you step. You almost crushed me!",
                                    "Fais attention où tu mets les pieds. Tu as failli m'écraser !",
                                    "足元を見て歩け！危うく踏まれるところだった！",
                                    "Cuidado onde você pisa. Quase me quebrou todinho!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "My apologies, I was looking at my map.",
                                    "Mille excuses, j'étais en train de regarder ma carte.",
                                    "ごめんなさい、地図を見ていたもので。",
                                    "Mil desculpas, estava olhando para o meu mapa.",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Exactly! You should look up from your map when you walk.",
                                    "Justement ! Tu devrais lever le nez de ta carte quand tu marches.",
                                    "だからこそ、歩くときは前を見るべきだ。",
                                    "Exatamente! Você não deveria olhar para o seu mapa enquanto anda.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You're right, thank you and have a good day!",
                                    "Vous avez raison, merci et bonne journée !",
                                    "その通りだ、ありがとう、良い一日を！",
                                    "Está certo, obrigado e tenha um bom dia!",
                                }
                            }
                        },

                        {
                            .character = CharacterChataigne,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Wait a minute, you won't get away that easily!",
                                    "Attends un peu, tu ne vas pas t’en tirer comme ça !",
                                    "ちょっと待て、そんな簡単に逃げられると思うなよ！",
                                    "Espera aí, você não vai se safar tão fácil!",
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
                    UseInstantCpu(0.5f),
                    .isFastWhenGoodHandIsFound = true,
                    .straightScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "*Cough cough*",
                                    "*Tousse tousse*",
                                    "*ゴホゴホ*",
                                    "*Cof-cof*",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Why is there so much smoke around here?",
                                    "Pourquoi y a-t-il autant de fumée par ici ?",
                                    "どうしてこんなに煙が立っているんだ？",
                                    "Porque tem tanta fumaça por aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hola amigo! I'm El Joker, magician.",
                                    "Hola amigo ! Je suis El Joker, magicien.",
                                    "やあアミーゴ！私はエル・ジョーカーだ。",
                                    "Hola amigo! Eu sou o El Joker, mágico.",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Look at my new magic trick… smoke without fire!",
                                    "Regarde mon nouveau tour de magie… la fumée sans feu !",
                                    "新しい手品を見てくれ…火のない煙だ！",
                                    "Veja meu novo truque… fumaça sem fogo!",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Yah!",
                                    "Yah !",
                                    "ヤァ！",
                                    "Iááá!",
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
                                    "ボーン！",
                                    "Bum!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "*Cough cough*",
                                    "*Tousse tousse*",
                                    "*ゴホゴホ*",
                                    "*Cof-cof*",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "It looks very impressive, but with all this smoke, I can't see anything!",
                                    "Ça a l'air très impressionnant mais avec toute cette fumée, je ne vois rien !",
                                    "とても驚いたが、この煙で何も見えない！",
                                    "Parece impressionante, mas com toda essa fumaça não consigo ver nada!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "*Cough cough*",
                                    "*Tousse tousse*",
                                    "*ゴホゴホ*",
                                    "*Cof-cof*",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterCaroline,
                .background = BackgroundCaveEntrance,
                .pin = { .x = 326, .y = 220 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isFastWhenAboveTarget = true,
                    .preferSides = true,
                    .straightScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Caroline!",
                                    "Caroline !",
                                    "キャロライン！",
                                    "Caroline!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Oh, hi Clovis. What are you doing here?",
                                    "Tiens, salut Clovis. Qu'est-ce que tu fais là ?",
                                    "あら、こんにちはクロヴィス。ここで何をしてるの？",
                                    "Ah, oi Clóvis. O que você tá fazendo aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Ah, Caroline! Mr Crocoford told me you were lost in the forest. He said you'd be here, he even drew me a map to help me find you!",
                                    "Ah, Caroline ! Monsieur Crocoford m'a dit que tu t'étais perdue dans la forêt. Il m'a dit que tu serais là, il m'a même dessiné une carte pour te retrouver !",
                                    "ああ、キャロライン！クロコフォード先生が言ってたんだ、君が森で迷子になってるって。君を見つけるために、この地図を描いてくれたんだ！",
                                    "Bem, Caroline! O senhor Crocoford me contou que você estava perdida na floresta. Ele disse que você estaria aqui, e até desenhou um mapa para me ajudar a te encontrar.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Lost? Oh, no, no, I'm not lost at all.",
                                    "Perdue ? Ah, non, non, je ne suis pas perdue du tout.",
                                    "迷子？いや、いや、私は迷子じゃないわ。",
                                    "Perdida? Ah não, não, eu não tô nem um pouco perdida.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Actually, I… I was just going for a little walk, for health.",
                                    "En fait, je… je faisais juste une petite promenade, pour la santé.",
                                    "実は、ちょっと健康のために散歩していただけよ。",
                                    "Na verdade, Eu… Eu só sai pra passear, faz bem pra saúde.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But… but Mr. Crocoford told me you were really lost. He said it was a rescue mission.",
                                    "Mais… mais monsieur Crocoford m'a dit que tu t'étais vraiment perdue. Il a dit que c'était une mission de sauvetage.",
                                    "でも… でもクロコフォード先生が本当に君が迷子になったって言ってたんだ。助けに行ってくれって。",
                                    "Mas… Mas o senhor Crocoford disse que você estava totalmente perdida. Ele disse que era uma missão de resgate.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Well, thank you very much, Clovis! You found me, so I'm not lost anymore.",
                                    "Eh bien merci beaucoup Clovis ! Tu m'as retrouvée, je ne suis donc plus perdue.",
                                    "まあ、ありがとう、クロヴィス！あなたが見つけてくれたから、もう迷子じゃないわ。",
                                    "Bem, muito obrigada, Clóvis! Você me encontrou, então não tô mais perdida.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "But I have to go, I have to visit this cave…",
                                    "Mais il faut que je te laisse, je dois visiter cette grotte…",
                                    "でも行かなきゃ、この洞窟を見に行かなきゃいけないの…",
                                    "Mas eu tenho que ir, tenho que visitar essa caverna…",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For… for…",
                                    "Pour… pour…",
                                    "あの… あの…",
                                    "Por… Por… ",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For health reasons!",
                                    "Pour la santé !",
                                    "健康のために！",
                                    "Por motivos de saúde!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Great! I'm glad I could help you!",
                                    "Super ! Je suis content de t'avoir aidée !",
                                    "それは良かった！君を助けられて嬉しいよ！",
                                    "Ótimo! Fico feliz em poder ajudar!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterShadow,
                .background = BackgroundCave,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.5f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Caroline? Are you here?",
                                    "Caroline ? Tu es là ?",
                                    "キャロライン？そこにいるの？",
                                    "Caroline? Você está aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "It's so dark around here.",
                                    "Il fait tout noir par ici.",
                                    "ここはすごく暗いな。",
                                    "Está tudo tão escuro.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What are those glowing dots?",
                                    "Tiens, que sont ces points lumineux ?",
                                    "あの光る点は何だ？",
                                    "O que são aqueles pontinhos brilhantes?",
                                }
                            }
                        },

                        {
                            .character = CharacterShadow,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Gao!",
                                    "Gao !",
                                    "ガオ！",
                                    "Gao!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Gao?",
                                    "Gao ?",
                                    "ガオ？",
                                    "Gao?",
                                }
                            }
                        },

                        {
                            .character = CharacterShadow,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Gao! Gao!",
                                    "Gao ! Gao !",
                                    "ガオ！ガオ！",
                                    "Gao! Gao!",
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
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Phew, finally outside!",
                                    "Ouf, enfin dehors !",
                                    "ふぅ、やっと外に出た！",
                                    "Ufa, finalmente fora!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Caroline and Mr. Crocoford? What are you doing here?",
                                    "Caroline et M. Crocoford ? Que faites-vous là ?",
                                    "キャロラインとクロコフォード先生？ここで何してるの？",
                                    "Caroline e senhor Crocoford? O que vocês estão fazendo aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hi ho!",
                                    "Coucou !",
                                    "ヤッホー♪",
                                    "Alou!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Are you alright, Clovis? Caroline told me she was attacked in the cave.",
                                    "Est-ce que ça va Clovis ? Caroline m'a dit qu'elle s'était faite attaquer dans la grotte.",
                                    "クロヴィス、大丈夫か？キャロラインが洞窟で襲われたと言っていた。",
                                    "Tudo bem com você, Clóvis? Caroline me disse que ela foi atacada na caverna.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Me too, glowing eyes jumped at me!",
                                    "Moi aussi, des yeux lumineux m'ont sauté dessus !",
                                    "僕もだ、光る目が僕に飛びかかってきた！",
                                    "Eu também, um par de olhos brilhantes veio pra cima de mim.",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I was so scared that I ran out of the cave!",
                                    "J'ai eu une telle frousse que je suis ressorti de la grotte !",
                                    "怖くて洞窟から逃げ出したんだ！",
                                    "Fiquei com tanto medo que sai correndo pra fora da caverna!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "No sign of the hat?",
                                    "Pas de trace du chapeau ?",
                                    "帽子の手がかりはなかったか？",
                                    "Sem sinal do chapéu?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "A hat? What hat?",
                                    "Un chapeau ? Quel chapeau ?",
                                    "帽子？何の帽子？",
                                    "Chapéu? que chapéu?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I have a very bad feeling about this…",
                                    "J'ai vraiment un très mauvais pressentiment…",
                                    "嫌な予感がする…",
                                    "Eu tenho um pressentimento muito ruim sobre isso…",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "To be continued…",
                                    "À suivre…",
                                    "続く…",
                                    "Continua…",
                                }
                            }
                        },
                    }
                }
            },
        }
    }
};
