//
//  story_caroline.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/03/2024.
//

#include "story_caroline.h"

#include "cpu.h"
#include "instantcpu.h"
#include "randomcpu.h"

const Story kCarolineStory = (Story) {
    .character = CharacterCaroline,
    .encounters = {
        .count = 12,
        .memory = (Encounter[12]) {
            {
                .opponent = CharacterCeres,
                .background = BackgroundSchoolCorridor,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((Cpu) {
                    UseRandomCpu(1.0f)
                }),
                .dialog = {
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Come on, Ceres, you have to help me study! Exams are coming up, and there are so many magic tricks to learn that it's giving me a headache.",
                                    "Allez Cérès, il faut que tu m'aides à réviser ! Les examens approchent, il y a tellement de tours de magie à apprendre que ça me donne la migraine.",
                                    "さあ、セレス、手伝って！試験の日が近づいているけど、トランプ手品を全然覚えてないの。勉強しないと大変なことになっちゃう！",
                                    "Vamos, Ceres, você tem que me ajudar a estudar! As provas tão chegando e tem tantas mágicas que eu preciso aprender, tá me dando dor de cabeça.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Coo?",
                                    "Rrrrou ?",
                                    "ポッポ？",
                                    "Có?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes, that's it. Look, I've prepared decks of cards. You can make them disappear by matching 3 or more cards of the same kind, straights or flushes.",
                                    "Oui, c'est ça. Regarde, j'ai préparé des paquets de cartes. Tu peux les faire disparaître en faisant des brelans, des carrés, des quintes ou des couleurs.",
                                    "見て、トランプを用意したの。ペア、スリーカード、フォーカード、ストレート、フラッシュ、どれでも消すことができるわ。",
                                    "Sim, é isso. Olha, eu preparei esses baralhos. Você pode fazer as cartas desaparecerem quando juntar 3 ou mais do mesmo valor, 5 ou mais do mesmo naipe ou sequências.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Coo coo.",
                                    "Rrrou rrou.",
                                    "ポッポ。",
                                    "Co-có.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Great! Let's get started!",
                                    "Super ! C'est parti !",
                                    "さあ始めよう！",
                                    "Ótimo! Vamos começar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Coo?",
                                    "Rrrou ?",
                                    "ポッポ？",
                                    "Có?",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterCrocoford,
                .background = BackgroundSchoolCorridor,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .sameKindScore = 1,
                }),
                .dialog = {
                    .count = 4,
                    .memory = (Phrase[4]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Still wandering the halls instead of studying, Miss Caroline? You should go to the library and review your lessons if you want a chance to pass the exam.",
                                    "Encore à trainer dans les couloirs au lieu de réviser mademoiselle Caroline ? Vous devriez aller réviser à la bibliothèque si vous souhaitez avoir une chance de réussir l'examen.",
                                    "キャロラインさん、また廊下でぶらついているのか？試験に合格したければ、図書館で勉強するべきですよ。",
                                    "Ainda zanzando pelos corredores ao invés de estudar, senhorita Caroline? Você deveria ir à biblioteca e revisar suas lições se gostaria de ter uma chance para passar no seu exame.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "(Oh no, it's Professor Crocoford).",
                                    "(Ah flûte, c'est le professeur Crocoford).",
                                    "（やだなあ、クロコフォード先生だ）。",
                                    "(Ah não, é o professor Crocoford).",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
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
                            .side = SideRight,
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
                .opponent = CharacterBelleplume,
                .background = BackgroundSchoolLibrary,
                .pin = { .x = 48, .y = 70 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .isFastWhenGoodHandIsFound = true,
                    .rowScore = 1,
                }),
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Something is sticking out of this book. It looks like a map!",
                                    "Tiens, quelque chose dépasse de ce livre. On dirait une carte !",
                                    "あれ、この本に何か挟まってる。地図みたい！",
                                    "Tem alguma coisa saindo desse livro. Parece um mapa!",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Coo?",
                                    "Rrrrou ?",
                                    "ポッポ？",
                                    "Có?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes, ma'am?",
                                    "Oui madame ?",
                                    "はい、先生？",
                                    "Sim, senhora?",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes, ma'am…",
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
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .isFastWhenGoodHandIsFound = true,
                    .randomFast = 50,
                    .sameSuitScore = 1,
                    .miss = 20,
                }),
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterClown,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I hope you enjoyed my show. Thank you, ladies and gentlemen! Thank you!",
                                    "J'espère que vous avez apprécié mon spectacle. Merci mesdames et messieurs ! Merci !",
                                    "私のショーを楽しんでくれましたか。ありがとうございます、皆さん！ありがとう！",
                                    "Espero que tenham gostado do meu show. Muito obrigado, senhoras e senhores. Obrigado!",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
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
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Bravo! Bravo! That was fantastic!",
                                    "Bravo ! Bravo ! C'était fantastique !",
                                    "ブラボー！ブラボー！素晴らしかったわ！",
                                    "Bravo! Bravo! Isso foi fantástico!",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Thanks, it's rare to have an audience these days. But? You have a hat… a dove… You're a magician!",
                                    "Merci, c'est rare d'avoir un public de nos jours. Mais ? Tu as un chapeau… une colombe… Tu es une magicienne !",
                                    "ありがとう、最近はなかなかショーを見てくれるお客さんがいません。君のその帽子、ハトを連れているということは…君はマジシャンだね！",
                                    "Obrigado, é raro ter público ultimamente. Mas? Você tem um chapéu… uma pomba… Você é uma mágica!",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "These days, everyone prefers magicians to clowns. No one comes to see my shows anymore, it's like I've become invisible.",
                                    "Aujourd'hui tout le monde préfère les magiciens aux clowns. Plus personne ne vient voir mes spectacles, on dirait que je suis devenu invisible.",
                                    "マジシャンのせいで僕は一人ぼっちになったようなものだよ。昔はお客さんの笑い声が広場中に響いていた。今では私のショーを見に来る人はいない。みんな謎めいた手品や華やかな演出の方が好きなんだ。",
                                    "Esses dias todo mundo prefere mágicos a palhaços. Ninguém vem mais ver meus shows, é como se eu não existisse.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Why don't you try something new? You could add magic tricks to your act?",
                                    "Pourquoi ne tentes-tu pas quelque chose de nouveau ? Tu pourrais peut-être intégrer des tours de magie dans ton spectacle ?",
                                    "新しいことに挑戦してみたらどう？ショーに手品を組み込んでみれば？",
                                    "Porque não tenta algo novo? Você não pode adicionar truques de mágica no seu ato?",
                                }
                            }
                        },

                        {
                            .character = CharacterClown,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Magic tricks? I won't stoop to that! I'll show you that clowns can compete!",
                                    "Des tours de magie ? Je ne vais pas m'abaisser à ça ! Je vais te prouver que les clowns peuvent rivaliser !",
                                    "手品のショー？そんなことはしない！君に証明してやる、ピエロだって面白いことができるのだから！",
                                    "Truques de mágica? Não vou me rebaixar a tanto! Vou te mostrar que palhaço também se garante!",
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
                    UseInstantCpu(1.0f),
                    .isAlwaysFast = true,
                    .straightScore = 1,
                    .miss = 20,
                }),
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "According to the map, it's this way, Ceres! ",
                                    "D'après la carte, c'est par là. Allons-y Cérès !",
                                    "地図によると、こちらの方向みたいだわ、セレス！",
                                    "Segundo o mapa é por aqui, Ceres!",
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
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Aaah! A talking tree!",
                                    "Aaaah ! Un sapin qui parle !",
                                    "ああっ！しゃべる木！",
                                    "Ai meu jesusinho! Uma árvore falante!",
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
                                    "What if we go around on the left side, Ceres?",
                                    "Et si on passait sur le côté gauche, Cérès ?",
                                    "じゃあ、左側を通ろうか、セレス？",
                                    "E se dermos a volta pela esquerda, Ceres?",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Coo coo.",
                                    "Rrouu rrou.",
                                    "ポッポ。",
                                    "Co-có.",
                                }
                            }
                        },

                        {
                            .character = CharacterSapique,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "No, no, no! Nobody passes! You shall not pass on the left side.",
                                    "Non non non ! Personne ne passe ! Vous ne passerez pas par le côté gauche.",
                                    "だめだ！左側でも断じて通さん！",
                                    "Nananinanão! Ninguém passa! Vocês não vão passar pela esquerda.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Alright! Come on, Ceres, let's go around on the right side then.",
                                    "D'accord ! Viens Cérès on passe par le côté droit alors.",
                                    "わかったよ。じゃあ右側から行こう。",
                                    "Sem problemas! Beleza, Ceres, vamos dar a volta pela direita então.",
                                }
                            }
                        },

                        {
                            .character = CharacterCeres,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Coo!",
                                    "Rrouu rrou !",
                                    "ポッポ！",
                                    "Có!",
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
                .opponent = CharacterOrelia,
                .background = BackgroundFloweryField,
                .pin = { .x = 290, .y = 60 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(1.0f),
                    .isAlwaysFast = true,
                    .sameSuitScore = 1,
                    .miss = 40,
                }),
                .dialog = {
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Have you come all this way to see me? I'm flattered.",
                                    "Vous avez fait tout ce chemin pour me voir ? Je suis flattée.",
                                    "私に会いに来てくれたの？光栄です。",
                                    "Você veio até aqui só pra me ver? Estou me achando a última bolacha do pacote.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "No, sorry, I'm looking for a…",
                                    "Non désolée, je suis à la recherche d'un…",
                                    "いいえ、ごめんなさい、本当は…",
                                    "Não, foi mal, tô procurando um…",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Don't say anything, I understand. You're mesmerised by my magnificent ears, and you don't know what to say, do you?",
                                    "Ne dites rien, je comprends. Vous êtes subjugué par mes magnifiques oreilles et vous ne savez pas quoi dire, n'est-ce pas ?",
                                    "何も言わなくてもいいわ、わかってる。私の可愛い耳に魅了されて、何も言えなくなってしまったんでしょう？",
                                    "Não diga mais nada, eu entendo. Você está encantada pelas minhas orelhas magníficas e não sabe o que dizer, não é mesmo?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh, uh, your ears are indeed… unique. But actually, I'm looking for…",
                                    "Oh, euh, vos oreilles sont en effet… uniques. Mais en réalité, je suis à la recherche de…",
                                    "あ、うん、確かに、あなたの耳は… ユニークだね。でも…",
                                    "Ah, hum, suas orelhas com certeza são… únicas. Mas, na verdade, eu tô procurando um…",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For?",
                                    "De ?",
                                    "なんでしょう？",
                                    "Um?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "… I'm looking for a magic hat!",
                                    "… je cherche un chapeau magique !",
                                    "…魔法の帽子を探しています！",
                                    "… Eu tô procurando um chapéu mágico!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "What? You want to put me in a hat and use me as an accessory for a magic trick? A rabbit as refined as me?",
                                    "Comment ? Vous voulez me mettre dans un chapeau et m'utiliser comme un accessoire pour un tour de magie ? Une lapine aussi rafinée que moi ?",
                                    "え？私を帽子に入れて、手品で使いたいの？こんなに可愛いうさぎを？",
                                    "O quê? Você quer me colocar num chapéu e me usar como um mero acessório para um truque de mágica? Uma coelha de alto nível como eu?",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It won't happen like that!",
                                    "Ça ne se passera pas comme ça !",
                                    "それはダメよ！",
                                    "Mas nem por um decreto!",
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
                    UseInstantCpu(1.0f),
                    .isAlwaysFast = true,
                    .sameSuitScore = 1,
                    .straightScore = 1,
                    .miss = 20,
                }),
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterPrince,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Do you have a moment, young lady? My loyal steed has disappeared, and I'm searching everywhere for her.",
                                    "Auriez-vous un instant, jeune demoiselle ? Mon compagnon a disparu, et je le recherche partout.",
                                    "お時間よろしいでしょうか、お嬢さん？ウマがいなくなって、どこにいるのか探しているところなのです。",
                                    "Você teria um momento, jovem dama? Minha leal companheira desapareceu e estou procurando por ela em todo lugar.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "What are you looking for?",
                                    "Que recherchez-vous ?",
                                    "何をお探しですって？",
                                    "O que você tá procurando?",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "As I just told you, I'm looking for my Horse. She is… tall, with a beautiful mane and a coat of dazzling white.",
                                    "Je cherche Cheval. Elle est… grande, avec une belle crinière et une robe d'un blanc éclatant.",
                                    "さっき言った通り、ウマを探しているんです。背が高くて美しい髪で、真っ白なドレス着てるんです。",
                                    "Como eu disse, estou procurando por minha Cavala. Ela é… alta, com uma crina linda e uma pelagem de um branco deslumbrante.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "You're looking for a horse?",
                                    "Vous cherchez un cheval ?",
                                    "馬を探しているの？",
                                    "Você tá procurando uma égua?",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Not a horse. Horse.",
                                    "Pas un cheval, Cheval.",
                                    "そうなんです。",
                                    "Não uma égua. Cavala.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But… You're a horse, aren't you?",
                                    "Mais… Vous êtes un cheval, non ?",
                                    "でも… あなた自身が馬なんじゃないですか？",
                                    "Mas… A companheira de um cavalo não é uma égua?",
                                }
                            }
                        },

                        {
                            .character = CharacterPrince,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "What an outrage! En garde!",
                                    "Quel outrage ! En garde !",
                                    "何を言っている！かかってこい！",
                                    "Como ousa?! En garde!",
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
                    .sameSuitScore = 1,
                    .miss = 30,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 9,
                    .memory = (Phrase[9]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "My stomach is growling… I should have packed a picnic.",
                                    "Mon ventre gargouille… J'aurais dû préparer un pique-nique avant de partir.",
                                    "お腹すいたなぁ… ピクニックの用意をしておくべきだった。お腹がグーグー鳴って…",
                                    "Minha barriga tá roncando… Eu deveria ter trazido um lanche.",
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
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "A sandwich in the middle of the forest? It's too late; I must be too hungry, I'm losing my mind.",
                                    "Un sandwich en plein milieu de la forêt ? C'est trop tard, je dois avoir trop faim, je suis en train de perdre la tête.",
                                    "森の中でサンドイッチ？もうだめ、すごくお腹が空いていて頭がおかしくなりそう。",
                                    "U-um sanduíche-iche! No meio da floresta?! Ó céus, é tarde demais pra mim. Eu devo estar morrendo de fome, ficando louca.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
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
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Or perhaps it's a stroke of luck? Dinner is served!",
                                    "Ou alors peut-être que c'est un gros coup de chance ? À table !",
                                    "パンだわ！すごいラッキーじゃない？いただきます！",
                                    "Ou pode ser minha sorte? Tá na mesa!",
                                }
                            }
                        },

                        {
                            .character = CharacterBaguetteMagique,
                            .expression = ExpressionWat,
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
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Just a small bite then. But where are you going? Come back!",
                                    "Juste un petit bout alors. Mais, où vas-tu ? Reviens !",
                                    "一切れだけでも。どこ行くの？戻ってきて！",
                                    "Só uma mordidinha então. Mas aonde você tá indo? Volta aqui!",
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

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionKirakira,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Come back!",
                                    "Reviens !",
                                    "待って！",
                                    "Volta Aqui!",
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
                    UseInstantCpu(0.69f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameSuitScore = 1,
                    .miss = 10,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hola magician! I am El Joker. What a pleasure to meet a colleague.",
                                    "Hola magicienne ! Je suis El Joker. Quel plaisir de rencontrer une collègue.",
                                    "ホーラ、マジシャン！私はエル・ジョーカーだ。同業者に会えて嬉しいね。",
                                    "Hola, mágica! Eu sou o El Joker. Um prazer em conhecer uma colega.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Nice to meet you, El Joker. Are you sure you're a magician too?",
                                    "Enchantée, El Joker. Tu es sûr d'être magicien aussi ?",
                                    "はじめまして、エル・ジョーカー。本当にマジシャンなの？",
                                    "Prazer em te conhecer, El Joker. Mas você tem certeza que também é um mágico?",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Absolutely! Let me show you one of my most famous tricks. See this rock? I'm going to make it disappear!",
                                    "Absolument ! Je peux te montrer l'un de mes plus fameux tour. Vois-tu ce rocher ? Je vais le faire disparaître !",
                                    "もちろん！私の最も有名な手品の一つを見せてあげよう。この岩を見てごらん！消し去ってやる！",
                                    "Com certeza! Deixa eu te mostrar um dos meus truques famosos. Vê essa pedra? Eu vou fazer ela desaparecer!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Really? I'm curious to see that.",
                                    "Vraiment ? Je suis curieuse de voir ça.",
                                    "本当に？それは見てみたいわ。",
                                    "Sério? Quero ver isso.",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Abracadabra…",
                                    "Abracadabra…",
                                    "アブラカダブラ…",
                                    "Abracadabra…",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "*Jumps and hits the rock*",
                                    "*S'élance et frappe le rocher*",
                                    "*飛び上がり、岩を叩く*",
                                    "*Pula e dá um soco na pedra*",
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
                                    "ドカン！",
                                    "Bum!",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "And there it is, gone!",
                                    "Et voilà, il a disparu !",
                                    "これで消えた！",
                                    "Tchanam! Sumiu!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Uh… It actually exploded into pieces…",
                                    "Euh… Il a plutôt explosé en mille morceaux…",
                                    "えっ… 消えたんじゃなくて岩を砕いただけなんだけど…",
                                    "Hum… Na verdade explodiu em pedaçinhos…",
                                }
                            }
                        },

                        {
                            .character = CharacterElJoker,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Exactly! Beautiful explosion, right? Strength, action, that's what makes magic exciting!",
                                    "Exactement ! Belle explosion, non ? La force, l'action, voilà ce qui rend la magie excitante !",
                                    "まさに！素晴らしいだろ？力、アクション、それが手品をエキサイティングにするんだ！",
                                    "Exatamente! Uma bela explosão, certo? Força, ação, é o que faz uma mágica emocionante!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Uh…",
                                    "Euh…",
                                    "ええと…",
                                    "Hum…",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterClovis,
                .background = BackgroundCaveEntrance,
                .pin = { .x = 326, .y = 220 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.69f),
                    .isFastWhenGoodHandIsFound = true,
                    .straightScore = 1,
                    .miss = 10,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 12,
                    .memory = (Phrase[12]) {
                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .side = SideLeft,
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
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "(I can't tell him I'm looking for a magic hat to cheat on exams!)",
                                    "(Je ne peux pas lui dire que je cherche un chapeau magique pour tricher aux examens !)",
                                    "（試験でカンニングするために魔法の帽子を探してるって言えない！）",
                                    "(Não posso contar pra ele que eu tô procurando um chapéu mágico pra colar nas provas!)",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .side = SideLeft,
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
                            .side = SideLeft,
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
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "For… for…",
                                    "Pour… pour…",
                                    "あの… あの…",
                                    "Por… Por…",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
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
                            .side = SideRight,
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
                .opponent = CharacterHat,
                .background = BackgroundCave,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.69f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Finally! I found it! The magic hat!",
                                    "Enfin ! Je l'ai trouvé ! Le chapeau magique !",
                                    "やっと見つけた！魔法の帽子だ！",
                                    "Finalmente! Eu encontrei! O chapéu mágico!",
                                }
                            }
                        },

                        {
                            .character = CharacterHat,
                            .expression = ExpressionShy,
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
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "It doesn't look exactly like what I expected, but…",
                                    "Il ne ressemble pas tout à fait à ce que j'imaginais…",
                                    "想像していたのとはちょっと違うけど…",
                                    "Não é bem o que eu esperava mas…",
                                }
                            }
                        },

                        {
                            .character = CharacterHat,
                            .expression = ExpressionShy,
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
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
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
                            .character = CharacterHat,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Gao! gao!",
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
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh, Professor Crocoford, what are you doing here?",
                                    "Tiens professeur Crocoford, que faites-vous là ?",
                                    "あら、クロコフォード先生、ここで何をしていらっしゃるのですか？",
                                    "Ó, professor Crocoford, o que você tá fazendo aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Congratulations, Miss Caroline, you have passed your exam!",
                                    "Félicitations mademoiselle Caroline, vous avez réussi votre examen !",
                                    "おめでとうございます、キャロラインさん、試験に合格しました！",
                                    "Parabéns senhorita Caroline, você passou no seu exame!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "My exam? But I didn't take any test?",
                                    "Mon examen ? Mais je n'ai passé aucun test ?",
                                    "試験？でも私は何のテストも受けていませんが？",
                                    "Meu exame? Mas eu não fiz nenhuma prova?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You have brilliantly passed the test! You went all the way to the cave and retrieved the hat.",
                                    "Vous avez brillamment réussi le test ! Vous êtes allée jusqu'à la caverne et vous avez récupéré le chapeau.",
                                    "あなたは見事に試験に合格しました！洞窟まで行って、帽子を取り戻しました。",
                                    "Você passou brilhantemente na prova! Você foi até a caverna e trouxe o chapéu.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "By the way, where is it?",
                                    "D'ailleurs où est-il ? ",
                                    "それで帽子はどこにあるのですか？",
                                    "E falando nisso, aonde ele está?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "The hat?",
                                    "Le chapeau ?",
                                    "帽子？",
                                    "O chapéu?",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionWat,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh no! It attacked me, so I hurried out.",
                                    "Flûte ! Il m'a attaqué alors je suis sortie précipitamment.",
                                    "しまった！帽子に襲われたので、急いで出てきました。",
                                    "Ah não! Ele me atacou então eu saí correndo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It attacked you?",
                                    "Il vous a attaqué ?",
                                    "あなたを襲ったのですか？",
                                    "Ele te atacou?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "And where is Clovis, by the way?",
                                    "Et d'ailleurs, où est Clovis ?",
                                    "それでクロヴィスはどこですか？",
                                    "E aonde está o Clóvis, por sinal?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I have a bad feeling…",
                                    "J'ai un mauvais pressentiment…",
                                    "嫌な予感がします…",
                                    "Eu tenho um mau pressentimento…",
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
