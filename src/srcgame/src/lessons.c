//
//  story_school.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 23/03/2024.
//

#include "lessons.h"

#include "phrase.h"

const LessonList kLessons = (LessonList) {
    .count =8,
    .memory = (Lesson[8]) {
            {
                .title = {
                    .text = {
                        "The Basics",
                        "Règles de base",
                        "基本",
                        "Regras básicas",
                    }
                },
                .objectives = {},
                .dialog = {
                    .count = 16,
                    .memory = (Phrase[16]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Welcome to the Robert-Houdin School of Magic! I am Crocoford, your head teacher and expert in card tricks.",
                                    "Bienvenue à l'école de magie Robert-Houdin ! Je suis Crocoford, votre professeur principal et maître en tours de cartes.",
                                    "ロベール＝ウーダン手品学校へようこそ！クロコフォードと申します。手品の教師です。",
                                    "Bem-vindos a Escola de Mágica Robert-Houdin! Eu sou Crocoford, seu diretor e mestre em truques de cartas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I will introduce you to the basics of card sleight of hand and dazzling your audience.",
                                    "Je vais vous présenter les bases pour escamoter les cartes et émerveiller l'assistance.",
                                    "トランプ手品で観客を楽しませる基本を紹介します。",
                                    "Vou apresentar os conceitos básicos da prestidigitação com cartas e como deslumbrar seu público.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "We will have one-on-one card manipulation duels.",
                                    "Nous allons faire des des duels d'escamotage en un contre un.",
                                    "この学校では一対一の手品の対決を行います。",
                                    "Nós teremos duelos um contra um de manipulação de cartas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "Duels are played with decks of 32 cards.",
                                    "Les duels se pratiquent avec des paquets de 32 cartes.",
                                    "対決は32枚入りのカードで行われます。",
                                    "Duelos são jogados com baralhos de 32 cartas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 8,
                            .line = {
                                .text = {
                                    "Each magician has their own game board.",
                                    "Chaque prestidigitateur dispose de son plateau de jeu.",
                                    "各マジシャンに自分のゲームボードがあります。",
                                    "Cada mágico tem seu próprio painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 4,
                            .line = {
                                .text = {
                                    "Your board is on the left.",
                                    "Votre plateau est à gauche.",
                                    "あなたのボードは左側にあります。",
                                    "O seu painel é o da esquerda.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 7,
                            .line = {
                                .text = {
                                    "Each turn, 2 cards will be dealt to you. The cards fall from the top of the board and you can move them sideways with the directional pad.",
                                    "À chaque tour, 2 cartes vous serons distribuées. Les cartes tombent du haut de plateau et vous pouvez les déplacer latéralement avec la croix directionnelle.",
                                    "毎回2枚のカードが配られます。ボードの上から落ちてきて、十字キーで横に移動できます。",
                                    "A cada turno você recebe 2 cartas. As cartas vão cair do topo do painel e você pode movê-las para o lado com os botões direcionais.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 7,
                            .line = {
                                .text = {
                                    "Here, I received the 9 of Clubs and the Queen of Hearts.",
                                    "Sur cette illustration, j'ai reçu le 9 de trèfle et la dame de cœur.",
                                    "この画像では、クラブの9とハートのクイーンを受け取りました。",
                                    "Aqui, eu recebi o 9 de paus e a rainha de copas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "You will need to form poker hands to make the cards disappear and avoid filling up your board.",
                                    "Vous devrez former des mains de poker pour faire disparaître les cartes et éviter de remplir votre plateau.",
                                    "ボードを埋めないようにするには、ポーカーハンドでトランプを消すことができます。",
                                    "Você precisa formar mãos de pôquer para fazer as cartas desaparecerem e evitar preencher o seu painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 6,
                            .line = {
                                .text = {
                                    "For example, here, by placing the Queen on top of the other two Queens, I form a three of a kind.",
                                    "Par exemple, ici, en déposant la reine sur les deux autres reines, je forme un brelan.",
                                    "例えば、ここではクイーンを他の2枚のクイーンの上に置くことで、スリーカードになります。",
                                    "Por exemplo, ao colocar a rainha em cima de outras duas rainhas, você forma uma trinca.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 3,
                            .line = {
                                .text = {
                                    "The three Queens disappear and the 9 of Clubs falls downward.",
                                    "Les trois reines disparaissent et le 9 de trèfle tombe vers le bas.",
                                    "3枚のクイーンが消え、クローバーの9が下に落ちます。",
                                    "As três rainhas desaparecem e o 9 de paus cai. ",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 5,
                            .line = {
                                .text = {
                                    "You will lose the duel if the column from which the cards fall fills up.",
                                    "Vous perdrez le duel si la colonne d'où tombe les cartes est remplie.",
                                    "カードが落ちるスペースがなくなりボードの上部を超えると対決に負けます。",
                                    "Você irá perder o duelo se a coluna da qual as cartas caem for preenchida.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 1,
                            .line = {
                                .text = {
                                    "A tip: learn to recognize the card symbols well. You will have little time while performing to distinguish them, so it is important to identify them quickly.",
                                    "Un conseil : apprenez à bien reconnaître les symboles des cartes. Vous aurez peu de temps pendant un tour pour les distinguer, donc il est important de pouvoir les identifier rapidement.",
                                    "ヒント：カードのマークをよく見ましょう。対決中にそれを見分ける時間は少ないので、素早く識別することが重要です。",
                                    "Dica: aprenda bem a reconhecer os naipes das cartas. Você vai ter pouco tempo para distinguir entre eles enquanto duela, então é importante identificá-los rapidamente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 1,
                            .line = {
                                .text = {
                                    "As you can see, the diamond and spade symbols on the cards we use are marked with an inner border.",
                                    "Comme vous pouvez le voir, les symboles carreau et pique des cartes que nous utilisons sont marqués d'une bordure intérieure.",
                                    "ご覧のとおり、使用するカードのダイヤとスペードには内側に線があります。",
                                    "Como pode ver, os naipes de ouro e espadas nas cartas são marcados com uma borda interna.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameStand,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "During a performance, this mark will help you to more quickly distinguish diamonds from hearts and spades from clubs.",
                                    "Pendant un spectacle, cette marque vous permettra de repérer plus rapidement les carreaux des cœurs ainsi que les piques des trèfles.",
                                    "対決中、このマークはダイヤとハート、スペードとクローバーを素早く識別するのに役立ちます。",
                                    "Durante uma apresentação, essa borda vai te ajudar a distinguir rapidamente ouros de copas e espadas de paus.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "That's all for today. In the next lesson, we will look at poker hands in more detail.",
                                    "C'est tout pour aujourd'hui. Dans la leçon suivante, nous verrons plus en détail les mains de poker.",
                                    "今日はこれで終わりです。次のレッスンでは、ポーカーハンドについて詳しく見ていきます。",
                                    "Isso é tudo por hoje. Na próxima aula iremos ver as mãos de pôquer com mais detalhes.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Poker Hands",
                        "Mains de poker",
                        "ポーカーハンド",
                        "Mãos de pôquer",
                    }
                },
                .objectives = {
                    .cardsToRemove = 20,
                },
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "In magic duels, we will use poker hands to make the cards disappear.",
                                    "Dans les duels de magie, nous allons utiliser des mains de poker pour faire disparaître les cartes.",
                                    "手品対決では、ポーカーハンドを使ってトランプを消します。",
                                    "Nos duelos de mágica nós iremos usar as mãos de pôquer para fazer as cartas desaparecerem.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "A poker hand is a combination of cards.",
                                    "Une main de poker est une combinaison de cartes.",
                                    "ポーカーハンドとは、カードの組み合わせのことです。",
                                    "A mão de pôquer é uma combinação de cartas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "The hands you can use are…",
                                    "Les mains que vous pourrez utilisez sont…",
                                    "使えるハンドは…",
                                    "As mãos que você pode usar são…",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameWalk,
                            .direction = 1,
                            .line = {
                                .text = {
                                    "Three of a kind and Four of a kind: 3 or more cards of the same value.",
                                    "Les brelans et les carrés : 3 cartes ou plus de la même valeur.",
                                    "スリーカードとフォーカード：同じ数字のカードを3枚またはそれ以上。",
                                    "Trincas e Quadras: 3 ou mais cartas do mesmo valor.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameWalk,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "Flush: 5 or more cards of the same suit. For example: 5 diamond cards next to each other.",
                                    "Les couleurs : 5 cartes ou plus du même symbole. Par exemple : 5 cartes de carreau côte à côte.",
                                    "フラッシュ：同じマークのカードを5枚以上。例えば、隣り合わせの5枚のダイヤのトランプ。",
                                    "Flush: 5 ou mais cartas do mesmo naipe. Por exemplo: 5 cartas de ouros lado a lado.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameWalk,
                            .direction = 3,
                            .line = {
                                .text = {
                                    "Straight: 5 cards in a row with consecutive values. For example: 8, 9, 10, J, Q.",
                                    "Les suites : 5 cartes côte à côte dont les valeurs se suivent. Par exemple : 8, 9, 10, J, Q.",
                                    "ストレート：隣り合わせの5枚の連続した値のカード。例えば、8、9、10、J、Q。",
                                    "Straight ou Sequência: 5 cartas de valores consecutivos. Por exemplo: 8, 9, 10, J, Q.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Each hand has its use. Flushes are weak but allow you to quickly clear a large number of cards from the board.",
                                    "Chaque main a son utilité. Les couleurs sont faibles mais permettent d'effacer rapidement un grand nombre de cartes du plateau.",
                                    "各ハンドにはそれぞれの利点があります。フラッシュは弱いですが、ボード上のより多くのカードを素早く消すことができます。",
                                    "Cada mão tem seu uso. Flushes são fracos mas permitem limpar rapidamente um número maior de cartas do painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Straights are difficult to assemble but are very strong.",
                                    "Les suites sont difficiles à assembler mais elles sont très fortes.",
                                    "ストレートは組み立てるのが難しいですが、一番強い組み合わせです。",
                                    "Sequências são difíceis de construir mas são muito poderosas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Three of a kind and Four of a kind are of medium strength but allow you to quickly prepare devastating combos.",
                                    "Les brelans et les carrés sont de force moyenne mais ils permettent de préparer rapidement des combos dévastateurs.",
                                    "スリーカードとフォーカードは中程度の強さですが、素早くに相手に連鎖攻撃を与えることができます。",
                                    "Trincas e Quadras são de força média, mas permitem preparar rapidamente combos devastadores.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "In the next lesson, we will see how making cards disappear affects your opponent.",
                                    "Dans la leçon suivante, nous verrons comment faire disparaître des cartes affecte votre adversaire.",
                                    "次のレッスンでは、カードを消すとどのように対戦相手に攻撃を与えるかを見ていきます。",
                                    "Na próxima aula, vamos ver como o desaparecimento de cartas afeta o seu oponente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "But for now, let's move on to practice. Try to make 20 cards disappear.",
                                    "Mais pour l'instant passons à la pratique. Essayez de faire disparaître 20 cartes.",
                                    "では、実践に移りましょう。カードを20枚消してみてください。",
                                    "Mas por hora vamos praticar. Tente fazer 20 cartas desaparecerem.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Sending Chips",
                        "Envoyer des jetons",
                        "チップを送る",
                        "Enviando fichas",
                    }
                },
                .objectives = {
                    .chipsToSend = 20,
                },
                .dialog = {
                    .count = 7,
                    .memory = (Phrase[7]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Did you notice the stars that fly away when you make cards disappear?",
                                    "Avez-vous remarqué les étoiles qui apparaissent lorsque vous faites disparaître des cartes ?",
                                    "カードを消すときに現れる星に気づきましたか？",
                                    "Você notou as estrelas que saem voando quando você faz cartas desaparecerem?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "These stars will make chips fall onto your opponent's board. It's the best way to unsettle them and win the game.",
                                    "Ces étoiles vont faire tomber des jetons sur le tableau de votre adversaire. C'est le meilleur moyen de le déstabiliser pour remporter la victoire.",
                                    "星は相手のボードにチップを落とします。こうやって相手を邪魔してより素早く勝つことができます。",
                                    "Essas estrelas farão fichas cairem no painel do seu oponente. É o melhor jeito de atrapalhá-los e vencer o jogo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "When you send chips, the number that will fall is displayed at the top of your opponent's board.",
                                    "Quand vous envoyer des jetons, le nombre qui va tomber s'affiche en haut du tableau de votre adversaire.",
                                    "チップを送ると、何個落ちるかは上部に表示されます。",
                                    "Quando você envia fichas, o número que irá cair será exibido no topo do painel do oponente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "1 single chip means that one chip will fall when the opponent places their cards.",
                                    "1 jeton simple signifie qu'un jeton tombera quand l'adversaire aura posé ses cartes. ",
                                    "チップ1枚は、相手がカードを置くときに1枚のチップが落ちることを意味します。",
                                    "Uma única ficha, significa que 1 ficha cai quando o oponente posicionar suas cartas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "A roll of chips represents a line, which is 6 chips.",
                                    "Un rouleau de jeton représente une ligne, soit 6 jetons.",
                                    "チップが数枚重なっている場合は1行、つまり6枚のチップを表します。",
                                    "Um rolo de fichas representa 1 linha, que são 6 fichas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "A star represents 6 lines, which is 36 chips.",
                                    "Une étoile représente 6 lignes, soit 36 jetons.",
                                    "星は6行、つまり36枚のチップを表します。",
                                    "Uma estrela representa 6 linhas, que são 36 fichas.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For this exercise, practice card sleight of hand and try to send 20 chips in as many turns as you need.",
                                    "Pour cet exercice, entraînez-vous à faire de l'escamotage et essayez d'envoyer 20 jetons en autant de tours que vous le souhaitez.",
                                    "この練習では、決まった回数のターンで20枚のチップを対戦相手に送るようにしてください。",
                                    "Para esse exercício, pratique seus truques de cartas e tente enviar 20 fichas em quantos turnos você precisar.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Removing Chips",
                        "Enlever jetons",
                        "チップを消す",
                        "Removendo fichas",
                    }
                },
                .objectives = {
                    .chipsToRemove = 6,
                    .boardCount = 18,
                    .board = "z____zzz__zzzzzzzz",
                },
                .dialog = {
                    .count = 10,
                    .memory = (Phrase[10]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "If you're up against a skilled opponent, it's very likely they will send chips your way.",
                                    "Si vous affrontez un adversaire habile, il est fort probable qu'il vous envoie des jetons.",
                                    "強い相手ほど、チップを送ってくる回数が多くなります。",
                                    "Se você estiver duelando contra um oponente habilidoso, é bem provável que ele mande fichas no seu painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "To get rid of the chips, clear the cards next to them. They will disappear at the same time.",
                                    "Pour vous débarrasser des jetons, faites disparaître des cartes situées à côté d'eux. Ils disparaîtront en même temps.",
                                    "チップを消すには、チップの隣のカードを消すと消えます。",
                                    "Para se livrar de fichas, elimine as cartas próximas a elas. Elas desaparecerão ao mesmo tempo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "When there are only few chips on the field, try to use them to your advantage.",
                                    "Quand il y a peu de jetons sur le terrain, essayez de les utiliser à votre avantage.",
                                    "チップをうまく利用することもできます。",
                                    "Quando restarem apenas algumas fichas em jogo, tente usá-las ao seu favor.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "They can be useful for building combos.",
                                    "Ils peuvent être utile pour construire des combos.",
                                    "連鎖攻撃を組むのに役立ちます。",
                                    "Elas podem ser úteis para fazer combos.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "When there are a lot of chips, you have several solutions to clean up.",
                                    "Quand il y a beaucoup de jetons, vous avez plusieurs solutions pour faire du ménage.",
                                    "チップがたくさんあるときは、全部消すための解決策がいくつかあります。",
                                    "Quando há muitas fichas, você tem várias soluções para eliminá-las.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Making a flush by putting cards of the same suit next to each other can make a lot of cards and chips disappear at once.",
                                    "Assembler des cartes du même symbole permet d'éliminer beaucoup de jetons d'un coup.",
                                    "同じトランプのマークのカードを隣り合わせにしてフラッシュを作ると、大量のカードとチップを一度に消すことができます。",
                                    "Ao fazer um flush colocando cartas do mesmo naipe próximas umas as outras, pode fazer várias cartas e fichas desaparecerem de uma vez.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Three of a kind also helps to quickly reduce clutter.",
                                    "Faire des brelans permet aussi de réduire légèrement l'encombrement rapidement.",
                                    "スリーカードも早くチップを消すのに役立ちます。",
                                    "Trincas também ajudam a limpar a bagunça rapidamente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "With practice, you'll get better and better at getting out of difficult situations.",
                                    "Avec la pratique vous arriverez de mieux en mieux à vous sortir des situations difficiles.",
                                    "練習すれば、困難な状況から抜け出すのがどんどんうまくなります。",
                                    "Com um pouco de prática, você vai melhorar cada vez mais em sair de situações difíceis.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Don't panic and do your best!",
                                    "Ne paniquez pas et faites de votre mieux !",
                                    "慌てずに頑張ってください！",
                                    "Não entre em pânico e faça o seu melhor!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Try to remove 6 chips from the board.",
                                    "Essayez de faire disparaître 6 jetons du terrain.",
                                    "ボードからチップを6枚を消してみてください。",
                                    "Tente remover 6 fichas do painel.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Countering",
                        "Contrer",
                        "カウンター",
                        "Contra-ataque",
                    }
                },
                .objectives = {
                    .chips = 35,
                    .chipsToSend = 1,
                    .boardCount = 12,
                    .board = "caEHjnCIMpbf",
                    .deckCount = 12,
                    .deck = "iAkKPhmeNFBJ",
                },
                .dialog = {
                    .count = 5,
                    .memory = (Phrase[5]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "There is a more effective way to get rid of chips: act before they fall on your board.",
                                    "Il existe une méthode plus efficace pour éliminer les jetons : agir avant qu'ils n'arrivent sur votre tableau.",
                                    "チップを消すにはもっと効果的な方法があります。ボードに落ちる前に消すことです。",
                                    "Existe uma maneira mais efetiva de se livrar das fichas: agir antes que elas caiam no seu painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "When chips are about to fall, you can counter them by making cards disappear from your board.",
                                    "Quand des jetons sont prêts à tomber, vous pouvez les contrer en faisant disparaître des cartes sur votre terrain.",
                                    "チップが落ちそうになったら、カードを自分のボードから消して対抗します。",
                                    "Quando as fichas estão prestes a cair, você pode contra-atacar ao fazer cartas desaparecerem do seu painel.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Countering will delay the fall of the chips for one round. If you can make more cards disappear in the following rounds, you can prevent the chips from falling altogether.",
                                    "Contrer retarde d'un tour la chute des jetons. Si vous parvenez à faire disparaître d'autres cartes aux tours suivants, vous pouvez éviter complètement la chute des jetons.",
                                    "チップが落ちる前に消すと、チップの落下を1回遅らせることができます。次の回でさらにカードを消すことができれば、チップの落下を完全に防ぐことができます。",
                                    "Contra-atacar atrasará a queda de fichas por uma rodada. Se você fizer mais cartas desaparecerem nas próximas rodadas, você pode evitar que as fichas caiam de vez.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Moreover, if your sleight of hand creates more chips than are waiting to fall, the power of your magic trick will be multiplied by 1.5!",
                                    "De plus, si votre escamotage génère plus de jetons qu'il n'y en a en attente, la puissance de votre tour de magie sera multipliée par 1,5 !",
                                    "さらに、あなたの手品で落下するはずのチップの枚数よりも多くのチップを作ることができれば、手品の威力は1.5倍になります！",
                                    "E ainda por cima, se suas mágicas criarem mais fichas das que estão por cair, os poderes dos seus truques serão multiplicados por 1.5!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For the next exercise, try to counter as much as possible to avoid being overwhelmed and send at least one chip to the opponent.",
                                    "Pour le prochain exercice, essayez de contrer au maximum pour ne pas vous retrouver submergé et envoyez au moins un jeton à l'adversaire.",
                                    "次の練習では、できるだけチップが落ちてくる前にカウンター攻撃をし、少なくとも1枚のチップを相手に送るようにしましょう。",
                                    "Para o próximo exercício, tente contra-atacar o máximo possível para evitar ficar sobrecarregado e tente enviar pelo menos uma ficha para o oponente.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Making Combos",
                        "Faire des combos",
                        "連鎖攻撃を作る",
                        "Combos",
                    }
                },
                .objectives = {
                    .chipsToSend = 99,
                    .comboToDo = 7,
                    .boardCount = 42,
                    .board = "____P_____O____pf_j__gF_i_HGLhACmeDnJbKkMl",
                    .deckCount = 2,
                    .deck = "ab",
                },
                .dialog = {
                    .count = 4,
                    .memory = (Phrase[4]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "To send more chips at once, you will need to master combos.",
                                    "Pour envoyer plus de jetons d'un coup, il vous faudra maîtriser les combos.",
                                    "一度に多くのチップを対戦相手に送るには、連鎖攻撃をマスターする必要があります。",
                                    "Para enviar mais fichas simultaneamente, você precisará dominar os combos.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Combos are chain reactions that occur when the cards that fall after a sleight of hand disappear in turn.",
                                    "Les combos sont des disparitions en chaîne qui se produisent lorsque les cartes qui tombent après un escamotage disparaissent à leur tour.",
                                    "連鎖攻撃とは、カードを消した後に落ちたカードが順番に消えていく連鎖反応のことです。",
                                    "Combos são reações em cadeia que ocorrem quando as cartas que caem após um truque de desaparecimento continuam a desaparecer em sequência.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "During a combo, each chain reaction increases a multiplier that is applied to the number of chips sent. The higher the combo, the more damage you deal.",
                                    "Pendant un combo, chaque réaction en chaîne augmente un multiplicateur qui est appliqué au nombre de jetons envoyés. Plus le combo est élevé et plus vous infligez de dégâts.",
                                    "連鎖反応するたびに、相手に送ったチップの枚数にかけられる倍率が上がり、与えるダメージも大きくなります。",
                                    "Durante um combo, cada reação em cadeia aumenta o multiplicador que é aplicado ao número de fichas enviadas. Quanto maior o combo, maior o dano causado.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "In this exercise, try to make a 7-step combo to send more than 100 chips to your opponent.",
                                    "Dans cet exercice, essayez de faire un combo à 7 étages pour envoyer plus de 100 jetons.",
                                    "この練習では、7段階の連鎖攻撃で相手に100枚以上のチップを送ることに挑戦してください。",
                                    "Nesse exercício, tente fazer um combo de 7 truques para enviar mais de 100 fichas ao seu oponente.",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "Adapting Tactics",
                        "Adapter son jeu",
                        "相手に適応する",
                        "Estratégias",
                    }
                },
                .objectives = {
                    .victoriesToUnlock = 1,
                },
                .dialog = {
                    .count = 13,
                    .memory = (Phrase[13]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Sometimes, you will encounter opponents who play much faster than you.",
                                    "Parfois vous allez rencontrer des adversaires qui jouent beaucoup plus vite que vous.",
                                    "時には、自分よりもはるかに速くプレイする相手に出会うことがあります。",
                                    "Às vezes, você irá encontrar oponentes que jogam bem mais rápido que você.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Here are two strategies for dealing with these opponents.",
                                    "Voici deux stratégies pour contrer ces adversaires. ",
                                    "そのような相手に対抗するための2つの戦略があります。",
                                    "Aqui estão duas estratégias para lidar com esse tipo de oponente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "First strategy: play slowly!",
                                    "Première stratégie : jouez lentement !",
                                    "第一の戦略は、ゆっくりプレイすることです！",
                                    "Primeira estratégia: jogue devagar!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "By doing this, you will delay the fall of chips onto your board. Meanwhile, the opponent might block themselves and lose.",
                                    "Vous retarderez ainsi la chute de jetons sur votre plateau. Pendant ce temps, l'adversaire risque de se bloquer lui-même et de perdre.",
                                    "これにより、自分のボードにチップが落ちるのを遅らせることができます。その間に、相手が自らのプレイをミスして、負けてしまう可能性があります。",
                                    "Ao fazer isso, você vai atrasar a queda de fichas no seu painel. E, enquanto isso, o oponente pode acabar se bloqueando e perdendo sozinho.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "This approach is very effective if your opponent is bad at clearing their board.",
                                    "Cette approche est très efficace si l'adversaire gère mal son plateau.",
                                    "この方法は、相手が自分のボードをうまく管理できない場合に非常に効果的です。",
                                    "Essa abordagem é muito eficaz se o seu oponente for ruim em limpar o próprio painel. ",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Second strategy: attack right at the start of the game!",
                                    "Deuxième stratégie : attaquez en tout début de partie !",
                                    "第二の戦略は、ゲームの序盤で攻撃を仕掛けることです！",
                                    "Segunda estratégia: ataque logo no início do duelo.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "This will prevent your opponent from building their hands and give you time to prepare stronger hands.",
                                    "Cela empêchera l'adversaire de construire ses mains et vous donnera du temps pour préparer vos propres attaques plus puissantes.",
                                    "これにより、相手が手札を作るのを妨げ、自分が強力な攻撃を準備する時間を確保できます。",
                                    "Isso impedirá que o oponente forme suas mãos e lhe dará mais tempo para preparar mãos mais fortes.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Also, take the time to observe how your opponent plays. Try to spot their favorite poker hand.",
                                    "Prenez aussi le temps d'observer comment joue votre adversaire. Essayez de repérer son attaque favorite.",
                                    "また、相手のプレイスタイルを観察する時間を取りましょう。相手のお気に入りの攻撃パターンを見つけることを試みてください。",
                                    "Use seu tempo também para observar como o seu oponente joga. Tente identificar a mão de pôquer preferida dele.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "For example, an opponent who is quick to chain three of a kind can be more easily destabilized if you play straights.",
                                    "Par exemple, un adversaire qui enchaîne rapidement les brelans peut être déstabilisé plus facilement si vous jouez des suites.",
                                    "例えば、素早くスリーカードを揃える相手は、自分がストレートを狙うことで混乱させることができます。",
                                    "Por exemplo, um oponente que é rápido para encadear trincas pode ser desestabilizado mais facilmente se você usar sequências.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "A small extra tip: remember that the cards come from a 32-card deck. If you've already gotten the four jacks, you won't get any more until that deck is finished.",
                                    "Petit conseil supplémentaire : rappelez-vous que les cartes proviennent d'un paquet de 32 cartes. Si vous avez déjà reçu les quatre valets, vous n’en recevrez plus tant que ce paquet ne sera pas terminé.",
                                    "もう一つのアドバイスですが、カードは32枚のデッキから配られることを覚えておいてください。もしすでに4枚のジャックを手にしているなら、そのデッキが終わるまで他のジャックは配られません。",
                                    "Uma dica extra: lembre-se que suas cartas vêm de um baralho de 32 cartas. Se você já tirou os quatro valetes, não irá receber mais nenhum até que esse baralho termine.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "With each new set of 32 cards, a new deck is opened and shuffled.",
                                    "À chaque nouvelle série de 32 cartes, un nouveau paquet est ouvert et mélangé.",
                                    "32枚の新しいカードセットごとに、新しいデッキが開かれてシャッフルされます。",
                                    "A cada novo conjunto de 32 cartas, um novo baralho é aberto e embaralhado.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "And also, prioritize Aces when you can because they send more chips to the opponent.",
                                    "De même, privilégiez les as quand vous le pouvez car ils enverront plus de jetons à l'adversaire.",
                                    "それに、できればエースを使ったほうがいいです。エースは相手にもっと多くのチップを送りますから。",
                                    "E também priorize o ás quando puder, pois ele envia mais fichas ao oponente.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I hope these tips help you improve. Good luck!",
                                    "J'espère que ces quelques conseils vous aiderons à progresser. Bonne chance !",
                                    "これらのアドバイスが、上達に役立つことを願っています。頑張ってください！",
                                    "Espero que essas dicas te ajudem a duelar melhor. Boa sorte!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .title = {
                    .text = {
                        "A Simple Trick",
                        "Un tour simple",
                        "簡単なトランプ手品",
                        "Truque simples",
                    }
                },
                .objectives = {
                    .victoriesToUnlock = 5,
                },
                .dialog = {
                    .count = 11,
                    .memory = (Phrase[11]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Today, I'm going to show you a simple trick that you can perform for your friends.",
                                    "Aujourd'hui, je vais vous montrer un tour simple que vous pourrez refaire à vos amis.",
                                    "今日は、友達に披露できる簡単な手品をお見せします。",
                                    "Hoje irei demonstrar um pequeno truque que você pode apresentar aos seus amigos.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameHurt,
                            .direction = 1,
                            .line = {
                                .text = {
                                    "Here are 6 cards. Pick one, but don't tell me which one.",
                                    "Voici 6 cartes, choisissez-en une sans me dire laquelle.",
                                    "ここに6枚のカードがあります。1枚選んでください。私にはどのカードか言わないように。",
                                    "Aqui estão 6 cartas. Escolha uma, mas sem me dizer qual.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameHurt,
                            .direction = 1,
                            .line = {
                                .text = {
                                    "Got it? You've chosen a card and memorized it?",
                                    "C'est bon ? Vous avez bien choisi et mémorisé votre carte ?",
                                    "選びましたか？ちゃんと覚えましたか？",
                                    "Tudo pronto? Já escolheu uma carta e memorizou?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Alright, now I'll shuffle the cards...",
                                    "Parfait, maintenant je vais mélanger les cartes…",
                                    "では、カードをよく混ぜます…",
                                    "Certo, agora vou embaralhar elas…",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I'll draw one... There were 6 cards, but now there are only 5.",
                                    "J'en tire une. Il y avait 6 cartes, il n'y en a maintenant plus que 5.",
                                    "1枚引きますね。最初は6枚ありましたが、今は5枚です。",
                                    "Vou tirar uma… Haviam 6 cartas, mas agora só há 5.",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Now, I'll spread the cards and...",
                                    "Regardez bien, j'étale les cartes et…",
                                    "次にカードを広げて…",
                                    "Agora vou mostrar as cartas que sobraram e…",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameHurt,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "There you go!",
                                    "Voilà !",
                                    "じゃん！",
                                    "Tcharam!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameHurt,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "Your card should have disappeared!",
                                    "Votre carte doit avoir disparu !",
                                    "あなたのカードが消えたはずです！",
                                    "A sua carta… sumiu!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .animation = AnimationNameHurt,
                            .direction = 2,
                            .line = {
                                .text = {
                                    "So, did I manage to make your card vanish?",
                                    "Alors, est-ce que j'ai réussi à faire disparaître votre carte ?",
                                    "どうですか？ちゃんと消えましたか？",
                                    "Então, consegui fazer sua carta desaparecer?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Impressive, isn't it?",
                                    "Impressionnant, n'est-ce pas ?",
                                    "驚きましたか？すごいでしょう？",
                                    "Impressionate, não é mesmo?",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "This trick is fairly simple. I hope you'll have fun figuring out how it works!",
                                    "Ce tour n'est pas très compliqué, j'espère que vous vous amuserez à découvrir comment il fonctionne !",
                                    "このトランプ手品はそれほど難しくありません。ぜひ、仕組みを解明して楽しんでください！",
                                    "Esse truque é bem simples. Espero que se divirta tentando entender como ele funciona.",
                                }
                            }
                        },
                    }
                }
            },
    }
};
