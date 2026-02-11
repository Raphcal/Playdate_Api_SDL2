//
//  story_melusine.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 19/07/2024.
//

#include "story_melusine.h"

#include "cpu.h"
#include "randomcpu.h"
#include "instantcpu.h"
#include "patterncpu.h"

const Story kMelusineStory = (Story) {
    .character = CharacterMelusine,
    .encounters = {
        .count = 8,
        .memory = (Encounter[8]) {
            {
                .opponent = CharacterOrelia,
                .background = BackgroundFloweryField,
                .pin = { .x = 290, .y = 60 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isAlwaysFast = true,
                    .sameSuitScore = 1,
                }),
                .dialog = {
                    .count = 18,
                    .memory = (Phrase[18]) {
                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Hello Earika, how is your little daycare doing?",
                                    "Bonjour Orelia, comment va ta petite garderie ?",
                                    "こんにちは、ミミ。あなたの保育園はどう？",
                                    "Olá Oreia, como vai a creche?",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hello, madam. It’s going well. I have two students now.",
                                    "Bonjour madame, ça va bien. J'ai deux élèves maintenant.",
                                    "こんにちは。元気です。今は二人の園児がいます。",
                                    "Olá senhora, a creche está ótima. Eu tenho duas alunas agora.",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hello, Melusine!",
                                    "Bonjour Mélusine !",
                                    "こんにちは、メリちゃん！",
                                    "Olá Melusina!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Hi!",
                                    "Bonjour ! ",
                                    "こんにちは！",
                                    "Oi!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Don’t be shy, Garouge, say hi!",
                                    "Ne fais pas ta timide Garouge, dis lui bonjour !",
                                    "ガルージュちゃん、恥ずかしがらないで、挨拶しなさい！",
                                    "Não se acanhe Lobelinha, diga oi!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Hello…",
                                    "Bonjour…",
                                    "こんにちは…",
                                    "Oi…",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Hello, Garouge. Are you the new student? I hope you get along well with my little Melusine.",
                                    "Bonjour Garouge. Tu es la nouvelle élève ? J'espère que tu t'entendras bien avec ma petite Mélusine.",
                                    "こんにちは、ガルージュちゃん。新しい生徒なのね？メリュジーヌと仲良くしてくれて。",
                                    "Olá Lobelinha. Você é a nova aluna? Espero que esteja se dando bem com minha pequena Melusina.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Nice to meet you! Do you like card tricks? I love them!",
                                    "Enchantée ! Est-ce que tu aimes les tours de carte ? Moi j'adore !",
                                    "はじめまして！トランプ手品は好き？私は大好きよ！",
                                    "Muito prazer! Você gosta de truques de carta? Eu amo!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Oh yes, I love them too! Let's' do card tricks together.",
                                    "Oh oui j'adore aussi ! On pourra en faire toutes les deux.",
                                    "ああ、私も大好き！一緒にやろう！",
                                    "Aham, eu também! Vamos fazer truques de carta juntas.",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Maybe later, kids. For now, it's time to say goodbye to Melusine’s mother.",
                                    "Tout à l'heure les enfants, pour l'instant il faut dire au revoir à la mère de Mélusine.",
                                    "後でね。今はメリちゃんのママにさよならを言いましょう。",
                                    "Talvez mais tarde, crianças. Agora é hora de dar tchau para a mãe da Melusina.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "To her mother? Goodbye, grandma!",
                                    "À sa mère ? Au revoir mère-grand !",
                                    "ママに？さよなら、おばあちゃん！",
                                    "A mãe dela? Tchau vovozinha!",
                                }
                            }
                        },

                        {
                            .character = CharacterBelleplume,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "See you tonight!",
                                    "À ce soir !",
                                    "またね！",
                                    "Até à noite!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Goodbye, Mom!",
                                    "Au revoir maman !",
                                    "またね、ママ！",
                                    "Tchau mamãe!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "So what are we gonna do this morning?",
                                    "Est-ce que vous avez une proposition d'activité pour ce matin ?",
                                    "さて、なんのアクティビティから始めますか？",
                                    "Então, o que vamos fazer essa manhã?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNeed,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Card tricks!",
                                    "Faire des tours de carte !",
                                    "トランプ手品から！",
                                    "Truques de carta!",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I should have guessed…",
                                    "J'aurai dû m'en douter…",
                                    "やっぱりね…",
                                    "Eu deveria ter imaginado…",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Alright, let’s go!",
                                    "C'est d'accord, allons-y !",
                                    "わかった、やりましょう！",
                                    "Certo, vamos começar!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yay!",
                                    "Youpi !",
                                    "やったー！",
                                    "Eba!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterPetitChaperonRouge,
                .background = BackgroundFloweryField,
                .pin = { .x = 295, .y = 85 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .straightScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 14,
                    .memory = (Phrase[14]) {
                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I did it!",
                                    "J'ai gagné !",
                                    "やった！",
                                    "Consegui!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Wow, Melusine, you’re really good!",
                                    "Ouah tu es super forte Mélusine !",
                                    "わあ、メリちゃんはすごく上手ね！",
                                    "Uau Melusina, você é muito boa nisso!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m not sure I want to duel with you after all.",
                                    "Je ne suis pas sûr d'avoir envie de faire un duel contre toi finalement.",
                                    "対決するのは少し怖いかも…",
                                    "Não sei se quero duelar com você depois disso.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’d like to make flower crowns instead!",
                                    "J'aimerai bien faire des couronnes de fleurs à la place !",
                                    "花冠が作りたいな！",
                                    "Acho que quero fazer coroas de flores!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "My mom is really good at that, but I prefer magic tricks.",
                                    "Ma maman sait super bien faire mais moi je préfère les tours de magie.",
                                    "ママは花冠がとても上手だけど、私はトランプ手品の方やりたい。",
                                    "Minha mãe é muito boa nisso, mas eu prefiro truques de mágica.",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Granny?",
                                    "Mère-grand ?",
                                    "おばあちゃん？",
                                    "A vovozinha?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Why do you call her grandmother?",
                                    "Pourquoi tu l'appelles mère-grand ?",
                                    "どうしてママのことをおばあちゃんと呼ぶの？",
                                    "Porque você chama ela de vovozinha?",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    " Because she’s grand, like really tall! And she’s a mom, so… Grandmother!",
                                    "Parce qu'elle est super grande ta maman !",
                                    "だって、あなたのママはとても大きいんだもん！",
                                    "Porque ela parece uma vovó, mas menorzinha!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
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
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Can you teach me to do magic tricks as well as you?",
                                    "Sinon tu peux m'apprendre à faire des tours de magie aussi bien que toi ?",
                                    "トランプ手品を教えてくれる？",
                                    "Você pode me ensinar alguns truques de mágica?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Yes, of course!",
                                    "Oui, bien sûr !",
                                    "もちろん！",
                                    "Sim, com certeza!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "And also, I want to prepare lots of new tricks to amaze my dad!",
                                    "Et aussi, je veux préparer plein de nouveaux tours pour épater mon papa !",
                                    "そして新しい手品をいっぱい考えて、パパをびっくりさせたいの！",
                                    "Eu também quero preparar vários truques novos para surpreender meu papai!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNeed,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "He’s the best at magic tricks, so I want to invent new ones to impress him!",
                                    "C'est le plus fort en tours de magie alors je veux inventer de nouveaux tours pour l'impressionner !",
                                    "パパは手品が一番上手だから、私はすごい手品を考えたい！",
                                    "Ele é o melhor mágico do mundo, então quero inventar truques novos para impressionar ele!",
                                }
                            }
                        },

                        {
                            .character = CharacterPetitChaperonRouge,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Awesome! Let's go!",
                                    "Super ! On commence ?",
                                    "すごいね！始めよう！",
                                    "Legal! Vamos nessa!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterCheval,
                .background = BackgroundForest,
                .pin = { .x = 290, .y = 110 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .preferSides = true,
                    .straightScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 14,
                    .memory = (Phrase[14]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "…I’m really in a hurry.",
                                    "…suis vraiment pressé.",
                                    "…急いでいるんだ。",
                                    "…Estou com muita pressa.",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "That’s my dad’s voice!",
                                    "C'est la voix de mon papa !",
                                    "パパの声だ！",
                                    "É a voz do meu papai!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
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
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .effect = EffectShake,
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
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m sorry.",
                                    "Je vous prie de m'excuser.",
                                    "ごめんなさい。",
                                    "Desculpa.",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m looking for a hiding place, I’m being chased.",
                                    "Je cherche une cachette, je suis poursuivie. ",
                                    "隠れ場所を探しているんです。追われています。",
                                    "Estou procurando um esconderijo, tem alguém atrás de mim.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I can help you, but first, I need to see my dad!",
                                    "Je peux vous aider, mais d'abord il faut que j'aille voir mon papa !",
                                    "お手伝いしますよ。でもその前にパパを探さなきゃ！",
                                    "Eu posso te ajudar, mas primeiro preciso ver meu papai!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Sorry, Melusine, but your father has already left. He was in a big hurry.",
                                    "Désolé Mélusine mais ton père est déjà parti. Il était très pressé.",
                                    "ごめんね、メリちゃん。あなたのパパはもう行ってしまったの。すごく急いでいたよ。",
                                    "Desculpa Melusina, mas seu pai já foi embora. Ele estava com muita pressa.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh no…",
                                    "Ah flûte…",
                                    "ああ、残念…",
                                    "Ah não…",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionShy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It’s my fault. How can I make it up to you?",
                                    "C'est ma faute, que puis-je faire pour me faire pardonner ?",
                                    "私のせいです。どうやってお詫びすればいいでしょうか？",
                                    "A culpa é minha. Como posso te recompensar?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNeed,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "A magic duel!",
                                    "Un duel de magie !",
                                    "手品の対決をしましょう！",
                                    "Um duelo de mágica!",
                                }
                            }
                        },

                        {
                            .character = CharacterCheval,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Alright.",
                                    "C'est d'accord.",
                                    "いいですよ。",
                                    "Bora!",
                                }
                            }
                        },
                    }
                }
            },

            {
                .opponent = CharacterPaguro,
                .background = BackgroundFloweryField,
                .pin = { .x = 256, .y = 156 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameSuitScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 17,
                    .memory = (Phrase[17]) {
                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Earika, can I follow my dad into the forest to show him my new magic tricks?",
                                    "Orelia, est-ce que je peux suivre mon papa dans la forêt pour lui montrer mes nouveaux tours de magie ?",
                                    "ミミ、パパに新しい手品を見せに森に行ってもいい？",
                                    "Oreia, posso ir atrás do meu papai pela floresta pra mostrar minhas novas mágicas pra ele?",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionKirakira,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Wouldn’t you rather I show you a magic trick with my ears?",
                                    "Tu ne préfères pas que je te fasse un tour de magie avec mes oreilles ?",
                                    "ここで手品を勉強したほがよくない？",
                                    "Você não prefere que eu te mostre uma mágica com as minhas orelhas?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
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
                            .character = CharacterOrelia,
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
                            .character = CharacterOrelia,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Well, I understand…",
                                    "Bon je comprends…",
                                    "それはわかるのだけど…",
                                    "Tá, entendi…",
                                }
                            }
                        },

                        {
                            .character = CharacterOrelia,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Go ahead! I’m rooting for you!",
                                    "Vas-y ! Je suis de tout cœur avec toi !",
                                    "行ってらっしゃい！応援しているよ！",
                                    "Pode ir! Estou torcendo por você!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Thanks, Earika!",
                                    "Merci Orelia !",
                                    "ありがとう、ミミ先生！",
                                    "Obrigada Oreia!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionMeh,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "But which way did Dad go?",
                                    "Mais par où est allé papa ?",
                                    "でもパパはどっちに行ったの？",
                                    "Mas pra que lado meu papai foi?",
                                }
                            }
                        },

                        {
                            .character = CharacterPaguro,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Are you looking for the crocodile? He went that way!",
                                    "Tu cherches le crocodile ? Il est parti par là !",
                                    "ワニさんを探してるの？あっちに行ったよ！",
                                    "Você está procurando pelo crocodilo? Ele foi por ali!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Thanks!",
                                    "Merci !",
                                    "ありがとう！",
                                    "Obrigada",
                                }
                            }
                        },

                        {
                            .character = CharacterPaguro,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Before you go, could you help me?",
                                    "Avant d'y aller, est-ce que tu pourrais m'aider ?",
                                    "行く前に、少し手伝ってくれない？",
                                    "Antes de ir, poderia me ajudar?",
                                }
                            }
                        },

                        {
                            .character = CharacterPaguro,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I don’t have a shelter. Do you have something that could serve as a house for me?",
                                    "Je n'ai pas d'abri, est-ce que tu aurais quelque chose qui pourrait me servir de maison ?",
                                    "隠れ家がなくて困っているんだ。どこか隠れられるところを知らない？",
                                    "Eu não tenho um abrigo. Você tem algo que eu posso usar como uma casa?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I don’t think so.",
                                    "Je ne crois pas.",
                                    "知らないわ…",
                                    "Acho que não.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionHappy,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "How about my sandwich?",
                                    "Et pourquoi pas mon sandwich ?",
                                    "サンドイッチはどう？",
                                    "Que tal meu sanduíche?",
                                }
                            }
                        },

                        {
                            .character = CharacterPaguro,
                            .expression = ExpressionHappy,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "I’m not sure it’s a good idea, but it’s better than nothing!",
                                    "Je ne suis pas sûr que ce soit une bonne idée mais c'est mieux que rien !",
                                    "いいかどうかわからないけど、ないよりマシだね！",
                                    "Não tenho certeza se é uma boa idéia, mas é melhor que nada.",
                                }
                            }
                        },

                        {
                            .character = CharacterPaguro,
                            .expression = ExpressionKirakira,
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
                    }
                }
            },

            {
                .opponent = CharacterCaroline,
                .background = BackgroundCave,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 12,
                    .memory = (Phrase[12]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "There, I’ll place the hat here. Perfect.",
                                    "Voilà, je pose le chapeau ici. C'est parfait.",
                                    "よし、この帽子をここに置こう。これで完璧だ。",
                                    "Pronto, vou colocar o chapéu aqui. Perfeito.",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I saw Dad go into this cave, but it’s so dark, I can’t see well…",
                                    "J'ai vu entrer papa dans cette grotte mais c'est tout sombre, je ne vois pas bien…",
                                    "パパがこの洞窟に入っていくのを見たけど、中は暗くてよく見えない…",
                                    "Eu vi o papai entrar nessa caverna, mas tá tão escuro, não consigo enxergar direito…",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Oh, a hat…",
                                    "Tiens, un chapeau…",
                                    "あれ、帽子が置いてある…",
                                    "Oh, um chapéu…",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Wait, this is Mom’s hat!",
                                    "Mais ? C'est le chapeau de maman !",
                                    "あれ？これはママの帽子！",
                                    "Peraí, é o chapéu da mamãe!",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I’ll wait here for Dad. He’ll surely come back to get the hat.",
                                    "Je vais attendre papa ici, il va sûrement revenir pour chercher le chapeau.",
                                    "ここでパパを待っていよう。きっと帽子を取りに戻ってくる。",
                                    "Vou esperar pelo papai aqui. Tenho certeza que ele vai voltar pra pegar o chapéu.",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionHappy,
                            .side = SideRight,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "(She wants to take Mom’s hat? I need to protect it!)",
                                    "(Elle veut prendre le chapeau de maman ? Il faut que je le protège !)",
                                    "(彼女がママの帽子を取ろうとしている？守らなきゃ！)",
                                    "(Ela quer pegar o chapéu da mamãe? Eu preciso protegê-lo)",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Gao!",
                                    "Gao ! ",
                                    "ガオ！",
                                    "Gao!",
                                }
                            }
                        },

                        {
                            .character = CharacterCaroline,
                            .expression = ExpressionMeh,
                            .side = SideRight,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                .opponent = CharacterClovis,
                .background = BackgroundCave,
                .pin = { .x = 366, .y = 210 },
                .actor = (Actor *) &((InstantCpu) {
                    UseInstantCpu(0.1f),
                    .isFastWhenGoodHandIsFound = true,
                    .sameKindScore = 1,
                    .avoidCenter = true,
                }),
                .gameSpeed = 2.0f,
                .dialog = {
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Phew! I managed to save the hat!",
                                    "Ouf ! J'ai réussi à sauver le chapeau !",
                                    "よし！帽子を守れた！",
                                    "Ufa! Consegui salvar o chapéu!",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Caroline? Are you here?",
                                    "Caroline ? Tu es là ?",
                                    "キャロライン？ここにいるの？",
                                    "Caroline? Você está aqui?",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionNormal,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "It’s so dark around here.",
                                    "Il fait tout noir par ici.",
                                    "ここはすごく暗いな。",
                                    "Está tudo tão escuro.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "(Another hat thief?)",
                                    "(Encore un voleur de chapeau ?)",
                                    "(また帽子泥棒？)",
                                    "(Outro ladrão de chapéu?)",
                                }
                            }
                        },

                        {
                            .character = CharacterClovis,
                            .expression = ExpressionAngry,
                            .side = SideRight,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                .opponent = CharacterCrocoford,
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
                    .count = 8,
                    .memory = (Phrase[8]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionAngry,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "…still don’t know who attacked you in the cave.",
                                    "…savons toujours pas qui vous a attaqué dans la grotte.",
                                    "…誰が君たちを襲ったのか、まだ分からない。",
                                    "…ainda não sabemos quem os atacou na caverna.",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
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
                            .side = SideLeft,
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
                            .side = SideRight,
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
                            .side = SideRight,
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
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "I followed you, Dad. You didn’t let me show you my new magic trick when you stopped by Earika’s!",
                                    "Je t'ai suivi papa. Tu ne m'as pas laissé te montrer mon nouveau tour de magie quand tu es passé chez Orelia !",
                                    "パパについてきたの。ミミさんのところに寄った時に、私の新しい手品を見てくれなかったから！",
                                    "Eu te segui papai. Você não me deixou te mostrar meu novo truque quando passou pela creche da Oreia!",
                                }
                            }
                        },

                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "But quickly, I sense danger approaching…",
                                    "Mais rapidement, je sens qu'un danger approche…",
                                    "でも早く、危険が近づいている気がする…",
                                    "Mas rápido, eu sinto perigo se aproximando…",
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
                    .count = 6,
                    .memory = (Phrase[6]) {
                        {
                            .character = CharacterCrocoford,
                            .expression = ExpressionNormal,
                            .side = SideRight,
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
                            .character = CharacterCrocoford,
                            .expression = ExpressionWat,
                            .side = SideRight,
                            .line = {
                                .text = {
                                    "Nooooooo!",
                                    "Noooooooon !",
                                    "いやぁぁぁぁぁぁ",
                                    "Nããããooo!",
                                }
                            }
                        },

                        {
                            .character = CharacterNone,
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
                            .character = CharacterMelusine,
                            .expression = ExpressionNormal,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Are Dad and Mom racing?",
                                    "Papa et maman font la course ?",
                                    "パパとママが走ってる！",
                                    "O papai e a mamãe estão apostando uma corrida?",
                                }
                            }
                        },

                        {
                            .character = CharacterMelusine,
                            .expression = ExpressionNeed,
                            .side = SideLeft,
                            .line = {
                                .text = {
                                    "Awesome! I’m coming too!",
                                    "Super ! Moi aussi, j'arrive !",
                                    "私も走る！楽しい！",
                                    "Que legal! Esperem por mim!",
                                }
                            }
                        },
                    }
                }
            },
        }
    }
};
