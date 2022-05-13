// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GoogleNLDefinitions.generated.h"


/**
 * Represents a category returned from the text classifier.
 */
USTRUCT(BlueprintType)
struct FNLClassificationCategory
{
    GENERATED_BODY()

    // The name of the category representing the document, from the predefined taxonomy.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FString name = "";

    // The classifier's confidence of the category. Number represents how certain the classifier is that this category represents the given text.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        float confidence = 0.0f;

};

/**
 * Represents an output piece of text.
 */
USTRUCT(BlueprintType)
struct FNLTextSpan
{
    GENERATED_BODY()

    // The content of the output text.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FString content = "";

    // The API calculates the beginning offset of the content in the original document according to the EncodingType specified in the API request.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        float beginOffset = 0.0f;
};


/**
 * Represents the feeling associated with the entire text or entities in the text.
 */
USTRUCT(BlueprintType)
struct FNLSentiment
{
    GENERATED_BODY()

    // A non-negative number in the [0, +inf) range, which represents the absolute magnitude of sentiment regardless of score (positive or negative).
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        float magnitude = 0.0f;

    // Sentiment score between -1.0 (negative sentiment) and 1.0 (positive sentiment).
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        float score = 0.0f;

};


UENUM(BlueprintType)
enum class ENLEncodingType : uint8
{
    NONE = 0,
    UTF8,
    UTF16,
    UTF32
};


UENUM(BlueprintType)
enum class ENLEntityType : uint8
{
    UNKNOWN       = 0 UMETA(ToolTip ="Unknown"),
    PERSON        = 1 UMETA(ToolTip ="Person"),
    LOCATION      = 2 UMETA(ToolTip = "Location"),
    ORGANIZATION  = 3 UMETA(ToolTip = "Organization"),
    EVENT         = 4 UMETA(ToolTip = "Event"),
    WORK_OF_ART   = 5 UMETA(ToolTip = "Artwork"),
    CONSUMER_GOOD = 6 UMETA(ToolTip = "Consumer product"),
    OTHER         = 7 UMETA(ToolTip = "Other types of entities"),
    PHONE_NUMBER  = 8 UMETA(ToolTip = "Phone number. The metadata lists the phone number, formatted according to local convention"),
    ADDRESS       = 9 UMETA(ToolTip = "Address. The metadata identifies the street number and locality plus whichever additional elements appear in the text"),
    DATE          = 10 UMETA(ToolTip = "Date. The metadata identifies the components of the date"),
    NUMBER        = 11 UMETA(ToolTip = "Number"),
    PRICE         = 12 UMETA(ToolTip = "Price")
};


UENUM(BlueprintType)
enum class ENLMentionType : uint8
{
    TYPE_UNKNOWN = 0 UMETA(ToolTip = "Unknown"),
    PROPER       = 1 UMETA(ToolTip = "Proper name"),
    COMMON       = 2 UMETA(ToolTip = "Common noun (or noun compound)")
};



/**
 * Represents a mention for an entity in the text. Currently, proper noun mentions are supported.
 */
USTRUCT(BlueprintType)
struct FNLEntityMention
{
    GENERATED_BODY()

    // The mention text.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLTextSpan text;

    // The type of the entity mention.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLMentionType type = ENLMentionType::TYPE_UNKNOWN;

    /**
     * For calls to [documents.analyzeEntitySentiment][] or if AnnotateTextRequest.Features.extract_entity_sentiment is set to true,
     * this field will contain the sentiment expressed for this mention of the entity in the provided document.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLSentiment sentiment;
};

/**
 * Represents a phrase in the text that is a known entity, such as a person, an organization, or location.
 * The API associates information, such as salience and mentions, with entities.
 */
USTRUCT(BlueprintType)
struct FNLEntity
{
    GENERATED_BODY()

    // The representative name for the entity.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FString name;

    // The entity type.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLEntityType type = ENLEntityType::UNKNOWN;

    /**
     * Metadata associated with the entity.
     * For most entity types, the metadata is a Wikipedia URL (wikipedia_url) and Knowledge Graph MID (mid), if they are available.
     * For the metadata associated with other entity types, see the Type table below.
     * An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        TMap<FString, FString> metaData;

    /**
     * The salience score associated with the entity in the [0, 1.0] range.
     * The salience score for an entity provides information about the importance or centrality of that entity to the entire document text.
     * Scores closer to 0 are less salient, while scores closer to 1.0 are highly salient.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        float salience = 0.0f;

    // The mentions of this entity in the input document. The API currently supports proper noun mentions.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        TArray<FNLEntityMention> mentions;

    /**
     * For calls to [documents.analyzeEntitySentiment][] or if AnnotateTextRequest.Features.extract_entity_sentiment is set to true,
     * this field will contain the aggregate sentiment expressed for this entity in the provided document.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLSentiment sentiment;
};


// Represents a sentence in the input document.
USTRUCT(BlueprintType)
struct FNLSentence
{
    GENERATED_BODY()

    // The sentence text.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLTextSpan text;

    /**
     * For calls to [documents.analyzeSentiment][] or if AnnotateTextRequest.Features.extract_document_sentiment is set to true,
     * this field will contain the sentiment for the sentence.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLSentiment sentiment;
};


// The parse label enum for the token.
UENUM(BlueprintType)
enum class ENLLabel : uint8
{
    UNKNOWN      = 0    UMETA(ToolTip = "Unknown"),
    ABBREV       = 1    UMETA(ToolTip = "Abbreviation modifier"),
    ACOMP        = 2    UMETA(ToolTip = "Adjectival complement"),
    ADVCL        = 3    UMETA(ToolTip = "Adverbial clause modifier"),
    ADVMOD       = 4    UMETA(ToolTip = "Adverbial modifier"),
    AMOD         = 5    UMETA(ToolTip = "Adjectival modifier of an NP"),
    APPOS        = 6    UMETA(ToolTip = "Appositional modifier of an NP"),
    ATTR         = 7    UMETA(ToolTip = "Attribute dependent of a copular verb"),
    AUX          = 8    UMETA(ToolTip = "Auxiliary (non-main) verb"),
    AUXPASS      = 9    UMETA(ToolTip = "Passive auxiliary"),
    CC           = 10   UMETA(ToolTip = "Coordinating conjunction"),
    CCOMP        = 11   UMETA(ToolTip = "Clausal complement of a verb or adjective"),
    CONJ         = 12   UMETA(ToolTip = "Conjunct"),
    CSUBJ        = 13   UMETA(ToolTip = "Clausal subject"),
    CSUBJPASS    = 14   UMETA(ToolTip = "Clausal passive subject"),
    DEP          = 15   UMETA(ToolTip = "Dependency (unable to determine)"),
    DET          = 16   UMETA(ToolTip = "Determiner"),
    DISCOURSE    = 17   UMETA(ToolTip = "Discourse"),
    DOBJ         = 18   UMETA(ToolTip = "Direct object"),
    EXPL         = 19   UMETA(ToolTip = "Expletive"),
    GOESWITH     = 20   UMETA(ToolTip = "Goes with (part of a word in a text not well edited)"),
    IOBJ         = 21   UMETA(ToolTip = "Indirect object"),
    MARK         = 22   UMETA(ToolTip = "Marker (word introducing a subordinate clause)"),
    MWE          = 23   UMETA(ToolTip = "Multi-word expression"),
    MWV          = 24   UMETA(ToolTip = "Multi-word verbal expression"),
    NEG          = 25   UMETA(ToolTip = "Negation modifier"),
    NN           = 26   UMETA(ToolTip = "Noun compound modifier"),
    NPADVMOD     = 27   UMETA(ToolTip = "Noun phrase used as an adverbial modifier"),
    NSUBJ        = 28   UMETA(ToolTip = "Nominal subject"),
    NSUBJPASS    = 29   UMETA(ToolTip = "Passive nominal subject"),
    NUM          = 30   UMETA(ToolTip = "Numeric modifier of a noun"),
    NUMBER       = 31   UMETA(ToolTip = "Element of compound number"),
    P            = 32   UMETA(ToolTip = "Punctuation mark"),
    PARATAXIS    = 33   UMETA(ToolTip = "Parataxis relation"),
    PARTMOD      = 34   UMETA(ToolTip = "Participial modifier"),
    PCOMP        = 35   UMETA(ToolTip = "The complement of a preposition is a clause"),
    POBJ         = 36   UMETA(ToolTip = "Object of a preposition"),
    POSS         = 37   UMETA(ToolTip = "Possession modifier"),
    POSTNEG      = 38   UMETA(ToolTip = "Postverbal negative particle"),
    PRECOMP      = 39   UMETA(ToolTip = "Predicate complement"),
    PRECONJ      = 40   UMETA(ToolTip = "Preconjunt"),
    PREDET       = 41   UMETA(ToolTip = "Predeterminer"),
    PREF         = 42   UMETA(ToolTip = "Prefix"),
    PREP         = 43   UMETA(ToolTip = "Prepositional modifier"),
    PRONL        = 44   UMETA(ToolTip = "The relationship between a verb and verbal morpheme"),
    PRT          = 45   UMETA(ToolTip = "Particle"),
    PS           = 46   UMETA(ToolTip = "Associative or possessive marker"),
    QUANTMOD     = 47   UMETA(ToolTip = "Quantifier phrase modifier"),
    RCMOD        = 48   UMETA(ToolTip = "Relative clause modifier"),
    RCMODREL     = 49   UMETA(ToolTip = "Complementizer in relative clause"),
    RDROP        = 50   UMETA(ToolTip = "Ellipsis without a preceding predicate"),
    REF          = 51   UMETA(ToolTip = "Referent"),
    REMNANT      = 52   UMETA(ToolTip = "Remnant"),
    REPARANDUM   = 53   UMETA(ToolTip = "Reparandum"),
    ROOT         = 54   UMETA(ToolTip = "Root"),
    SNUM         = 55   UMETA(ToolTip = "Suffix specifying a unit of number"),
    SUFF         = 56   UMETA(ToolTip = "Suffix"),
    TMOD         = 57   UMETA(ToolTip = "Temporal modifier"),
    TOPIC        = 58   UMETA(ToolTip = "Topic marker"),
    VMOD         = 59   UMETA(ToolTip = "Clause headed by an infinite form of the verb that modifies a noun"),
    VOCATIVE     = 60   UMETA(ToolTip = "Vocative"),
    XCOMP        = 61   UMETA(ToolTip = "Open clausal complement"),
    SUFFIX       = 62   UMETA(ToolTip = "Name suffix"),
    TITLE        = 63   UMETA(ToolTip = "Name title"),
    ADVPHMOD     = 64   UMETA(ToolTip = "Adverbial phrase modifier"),
    AUXCAUS      = 65   UMETA(ToolTip = "Causative auxiliary"),
    AUXVV        = 66   UMETA(ToolTip = "Helper auxiliary"),
    DTMOD        = 67   UMETA(ToolTip = "Rentaishi (Prenominal modifier)"),
    FOREIGN      = 68   UMETA(ToolTip = "Foreign words"),
    KW           = 69   UMETA(ToolTip = "Keyword"),
    LIST         = 70   UMETA(ToolTip = "List for chains of comparable items"),
    NOMC         = 71   UMETA(ToolTip = "Nominalized clause"),
    NOMCSUBJ     = 72   UMETA(ToolTip = "Nominalized clausal subject"),
    NOMCSUBJPASS = 73   UMETA(ToolTip = "Nominalized clausal passive"),
    NUMC         = 74   UMETA(ToolTip = "Compound of numeric modifier"),
    COP          = 75   UMETA(ToolTip = "Copula"),
    DISLOCATED   = 76   UMETA(ToolTip = "Dislocated relation (for fronted/topicalized elements)"),
    ASP          = 77   UMETA(ToolTip = "Aspect marker"),
    GMOD         = 78   UMETA(ToolTip = "Genitive modifier"),
    GOBJ         = 79   UMETA(ToolTip = "Genitive object"),
    INFMOD       = 80   UMETA(ToolTip = "Infinitival modifier"),
    MES          = 81   UMETA(ToolTip = "Measure"),
    NCOMP        = 82   UMETA(ToolTip = "Nominal complement of a noun")
};


USTRUCT(BlueprintType)
struct FNLDependencyEdge
{
    GENERATED_BODY()
    
    /**
     * Represents the head of this token in the dependency tree.
     * This is the index of the token which has an arc going to this token.
     * The index is the position of the token in the array of tokens returned by the API method.
     * If this token is a root token, then the headTokenIndex is its own index.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        int32 headTokenIndex;

    // The parse label for the token.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLLabel label;
};


// The relationship between the action that a verb expresses and the participants identified by its arguments.
UENUM(BlueprintType)
enum class ENLVoice : uint8
{
    VOICE_UNKNOWN = 0 UMETA(ToolTip = "Voice is not applicable in the analyzed language or is not predicted."),
    ACTIVE        = 1 UMETA(ToolTip = "Active"),
    CAUSATIVE     = 2 UMETA(ToolTip = "Causative"),
    PASSIVE       = 3 UMETA(ToolTip = "Passive")
};


// Time reference.
UENUM(BlueprintType)
enum class ENLTense : uint8
{
    TENSE_UNKNOWN     = 0 UMETA(ToolTip = "Tense is not applicable in the analyzed language or is not predicted."),
    CONDITIONAL_TENSE = 1 UMETA(ToolTip = "Conditional"),
    FUTURE            = 2 UMETA(ToolTip = "Future"),
    PAST              = 3 UMETA(ToolTip = "Past"),
    PRESENT           = 4 UMETA(ToolTip = "Present"),
    IMPERFECT         = 5 UMETA(ToolTip = "Imperfect"),
    PLUPERFECT        = 6 UMETA(ToolTip = "Pluperfect")
};


// Reciprocal features of a pronoun.
UENUM(BlueprintType)
enum class ENLReciprocity : uint8
{
    RECIPROCITY_UNKNOWN = 0 UMETA(ToolTip = "Reciprocity is not applicable in the analyzed language or is not predicted."),
    RECIPROCAL          = 1 UMETA(ToolTip = "Reciprocal"),
    NON_RECIPROCAL      = 2 UMETA(ToolTip = "Non-reciprocal")
};


// This category shows if the token is part of a proper name.
UENUM(BlueprintType)
enum class ENLProper : uint8
{
    PROPER_UNKNOWN = 0 UMETA(ToolTip = "Proper is not applicable in the analyzed language or is not predicted."),
    PROPER         = 1 UMETA(ToolTip = "Proper"),
    NOT_PROPER     = 2 UMETA(ToolTip = "Not proper")
};


// The distinction between the speaker, second person, third person, etc.
UENUM(BlueprintType)
enum class ENLPerson : uint8
{
    PERSON_UNKNOWN   = 0 UMETA(ToolTip = "Person is not applicable in the analyzed language or is not predicted."),
    FIRST            = 1 UMETA(ToolTip = "First"),
    SECOND           = 2 UMETA(ToolTip = "Second"),
    THIRD            = 3 UMETA(ToolTip = "Third"),
    REFLEXIVE_PERSON = 4 UMETA(ToolTip = "Reflexive")
};


// Count distinctions.
UENUM(BlueprintType)
enum class ENLNumber : uint8
{
    NUMBER_UNKNOWN = 0 UMETA(ToolTip = "Number is not applicable in the analyzed language or is not predicted."),
    SINGULAR       = 1 UMETA(ToolTip = "Singular"),
    PLURAL         = 2 UMETA(ToolTip = "Plural"),
    DUAL           = 3 UMETA(ToolTip = "Dual")
};


// The grammatical feature of verbs, used for showing modality and attitude.
UENUM(BlueprintType)
enum class ENLMood : uint8
{
    MOOD_UNKNOWN     = 0 UMETA(ToolTip = "Mood is not applicable in the analyzed language or is not predicted."),
    CONDITIONAL_MOOD = 1 UMETA(ToolTip = "Conditional"),
    IMPERATIVE       = 2 UMETA(ToolTip = "Imperative"),
    INDICATIVE       = 3 UMETA(ToolTip = "Indicative"),
    INTERROGATIVE    = 4 UMETA(ToolTip = "Interrogative"),
    JUSSIVE          = 5 UMETA(ToolTip = "Jussive"),
    SUBJUNCTIVE      = 6 UMETA(ToolTip = "Subjunctive")
};


// Gender classes of nouns reflected in the behaviour of associated words.
UENUM(BlueprintType)
enum class ENLGender : uint8
{
    GENDER_UNKNOWN = 0 UMETA(ToolTip = "Gender is not applicable in the analyzed language or is not predicted."),
    FEMININE       = 1 UMETA(ToolTip = "Feminine"),
    MASCULINE      = 2 UMETA(ToolTip = "Masculine"),
    NEUTER         = 3 UMETA(ToolTip = "Neuter")
};

/**
 * Depending on the language, Form can be categorizing different forms of verbs, adjectives, adverbs, etc.
 * For example, categorizing inflected endings of verbs and adjectives or distinguishing between
 * short and long forms of adjectives and participles
 */
UENUM(BlueprintType)
enum class ENLForm : uint8
{
    FORM_UNKNOWN   = 0 UMETA(ToolTip = "Form is not applicable in the analyzed language or is not predicted."),
    ADNOMIAL       = 1 UMETA(ToolTip = "Adnomial"),
    AUXILIARY      = 2 UMETA(ToolTip = "Auxiliary"),
    COMPLEMENTIZER = 3 UMETA(ToolTip = "Complementizer"),
    FINAL_ENDING   = 4 UMETA(ToolTip = "Final ending"),
    GERUND         = 5 UMETA(ToolTip = "Gerund"),
    REALIS         = 6 UMETA(ToolTip = "Realis"),
    IRREALIS       = 7 UMETA(ToolTip = "Irrealis"),
    SHORT          = 8 UMETA(ToolTip = "Short form"),
    LONG           = 9 UMETA(ToolTip = "Long form"),
    ORDER          = 10 UMETA(ToolTip = "Order form"),
    SPECIFIC       = 11 UMETA(ToolTip = "Specific form")
};


/**
 * The grammatical function performed by a noun or pronoun in a phrase, clause, or sentence.
 * In some languages, other parts of speech, such as adjective and determiner,
 * take case inflection in agreement with the noun.
 */
UENUM(BlueprintType)
enum class ENLCase : uint8
{
    CASE_UNKNOWN   = 0 UMETA(ToolTip = "Case is not applicable in the analyzed language or is not predicted."),
    ACCUSATIVE     = 1 UMETA(ToolTip = "Accusative"),
    ADVERBIAL      = 2 UMETA(ToolTip = "Adverbial"),
    COMPLEMENTIVE  = 3 UMETA(ToolTip = "Complementive"),
    DATIVE         = 4 UMETA(ToolTip = "Dative"),
    GENITIVE       = 5 UMETA(ToolTip = "Genitive"),
    INSTRUMENTAL   = 6 UMETA(ToolTip = "Instrumental"),
    LOCATIVE       = 7 UMETA(ToolTip = "Locative"),
    NOMINATIVE     = 8 UMETA(ToolTip = "Nominative"),
    OBLIQUE        = 9 UMETA(ToolTip = "Oblique"),
    PARTITIVE      = 10 UMETA(ToolTip = "Partitive"),
    PREPOSITIONAL  = 11 UMETA(ToolTip = "Prepositional"),
    REFLEXIVE_CASE = 12 UMETA(ToolTip = "Reflexive"),
    RELATIVE_CASE  = 13 UMETA(ToolTip = "Relative"),
    VOCATIVE       = 14 UMETA(ToolTip = "Vocative")
};


// The characteristic of a verb that expresses time flow during an event.
UENUM(BlueprintType)
enum class ENLAspect : uint8
{
    ASPECT_UNKNOWN = 0 UMETA(ToolTip = "Aspect is not applicable in the analyzed language or is not predicted."),
    PERFECTIVE     = 1 UMETA(ToolTip = "Perfective"),
    IMPERFECTIVE   = 2 UMETA(ToolTip = "Imperfective"),
    PROGRESSIVE    = 3 UMETA(ToolTip = "Progressive")
};


// The part of speech tags enum.
UENUM(BlueprintType)
enum class ENLTag : uint8
{
    UNKNOWN = 0 UMETA(ToolTip = "Unknown"),
    ADJ     = 1 UMETA(ToolTip = "Adjective"),
    ADP     = 2 UMETA(ToolTip = "Adposition (preposition and postposition)"),
    ADV     = 3 UMETA(ToolTip = "Adverb"),
    CONJ    = 4 UMETA(ToolTip = "Conjunction"),
    DET     = 5 UMETA(ToolTip = "Determiner"),
    NOUN    = 6 UMETA(ToolTip = "Noun (common and proper)"),
    NUM     = 7 UMETA(ToolTip = "Cardinal number"),
    PRON    = 8 UMETA(ToolTip = "Pronoun"),
    PRT     = 9 UMETA(ToolTip = "Particle or other function word"),
    PUNCT   = 10 UMETA(ToolTip = "Punctuation"),
    VERB    = 11 UMETA(ToolTip = "Verb (all tenses and modes)"),
    X       = 12 UMETA(ToolTip = "Other: foreign words, typos, abbreviations"),
    AFFIX   = 13 UMETA(ToolTip = "Affix")
};


// Represents part of speech information for a token.
USTRUCT(BlueprintType)
struct FNLPartOfSpeech
{
    GENERATED_BODY()

    // The part of speech tag.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLTag tag = ENLTag::UNKNOWN;

    // The grammatical aspect.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLAspect aspect = ENLAspect::ASPECT_UNKNOWN;

    // The grammatical case.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLCase Case = ENLCase::CASE_UNKNOWN;

    // The grammatical form.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLForm form = ENLForm::FORM_UNKNOWN;

    // The grammatical gender.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLGender gender = ENLGender::GENDER_UNKNOWN;

    // The grammatical mood.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLMood mood = ENLMood::MOOD_UNKNOWN;

    // The grammatical number.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLNumber number = ENLNumber::NUMBER_UNKNOWN;

    // The grammatical person.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLPerson person = ENLPerson::PERSON_UNKNOWN;

    // The grammatical properness.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLProper proper = ENLProper::PROPER_UNKNOWN;

    // The grammatical reciprocity.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLReciprocity reciprocity = ENLReciprocity::RECIPROCITY_UNKNOWN;

    // The grammatical tense.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLTense tense = ENLTense::TENSE_UNKNOWN;

    // The grammatical voice.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        ENLVoice voice = ENLVoice::VOICE_UNKNOWN;
};


// Represents the smallest syntactic building block of the text.
USTRUCT(BlueprintType)
struct FNLToken
{

    GENERATED_BODY()

    // The token text.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLTextSpan text;

    // Parts of speech tag for this token.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLPartOfSpeech partOfSpeech;

    // Dependency tree parse for this token.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FNLDependencyEdge dependencyEdge;

    // Lemma of the token.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        FString lemma;
};


UENUM(BlueprintType)
enum class ENLDocumentType : uint8
{
    TYPE_UNSPECIFIED = 0,
    PLAIN_TEXT,
    HTML
};


// All available features for sentiment, syntax, and semantic analysis. Setting each one to true will enable that specific analysis for the input.
USTRUCT(BlueprintType)
struct FNLFeatures
{
    GENERATED_BODY()

    // Extract syntax information.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        bool extractSyntax = false;

    // Extract entities.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        bool extractEntities = false;

    // Extract document-level sentiment.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        bool extractDocumentSentiment = false;

    // Extract entities and their associated sentiment.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        bool extractEntitySentiment = false;

    // Classify the full document into categories. If this is true, the API will use the default model which classifies into a predefined taxonomy.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
        bool classifyText = false;
};


UENUM(BlueprintType)
enum class ENLLanguage : uint8
{
    ARABIC              = 0 UMETA(ToolTip = "Arabic  ar"),
    CHINISE_SIMPLIFIED  = 1 UMETA(ToolTip = "Chinese (Simplified)    zh"),
    CHINISE_TRADITIONAL = 2 UMETA(ToolTip = "Chinese (Traditional)   zh-Hant"),
    DUTCH               = 3 UMETA(ToolTip = "Dutch   nl"),
    ENGLISH             = 4 UMETA(ToolTip = "English en"),
    FRENCH              = 5 UMETA(ToolTip = "French  fr"),
    GERMAN              = 6 UMETA(ToolTip = "German  de"),
    INDONESIAN          = 7 UMETA(ToolTip = "Indonesian  id"),
    ITALIAN             = 8 UMETA(ToolTip = "Italian it"),
    JAPANESE            = 9 UMETA(ToolTip = "Japanese    ja"),
    KOREAN              = 10 UMETA(ToolTip = "Korean  ko"),
    POLISH              = 11 UMETA(ToolTip = "Polish  pl"),
    PORTUGUESE          = 12 UMETA(ToolTip = "Portuguese (Brazilian & Continental)    pt"),
    RUSSIAN             = 13 UMETA(ToolTip = "Russian ru"),
    SPANISH             = 14 UMETA(ToolTip = "Spanish es"),
    THAI                = 15 UMETA(ToolTip = "Thai    th"),
    TURKISH             = 16 UMETA(ToolTip = "Turkish tr"),
    VIETNAMESE          = 17 UMETA(ToolTip = "Vietnamese  vi"),
    LANGUAGE_UNKNOWN    = 18 UMETA(ToolTip = "Unknown")
};


UENUM(BlueprintType)
enum class ENLAPIMethod : uint8
{
    SYNTACTIC_ANALISIS = 0,
    ENTITY_ANALYSIS,
    SENTIMENT_ANALYSIS,
    ENTITY_SENTIMENT_ANALYSIS,
    CONTENT_CLASSIFICATION,
    ANNOTATE_TEXT
};


USTRUCT(BlueprintType)
struct FNLDocument
{
    GENERATED_BODY()

    // Required. If the type is not set or is TYPE_UNSPECIFIED, returns an INVALID_ARGUMENT error.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
    ENLDocumentType type = ENLDocumentType::PLAIN_TEXT;

    /*
        The language of the document (if not specified, the language is automatically detected).
        Both ISO and BCP-47 language codes are accepted. Language Support lists currently supported languages for each API method.
        If the language (either specified by the caller or automatically detected) is not supported by the called API method,
        an INVALID_ARGUMENT error is returned.
    */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
    ENLLanguage language = ENLLanguage::ENGLISH;

    // The content of the input in string format. Cloud audit logging exempt since it is based on user data.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
    FString content = "";

    /*
        The Google Cloud Storage URI where the file content is located.
        This URI must be of the form: gs://bucket_name/object_name. For more details,
        see https://cloud.google.com/storage/docs/reference-uris.
        NOTE: Cloud Storage object versioning is not supported.
    */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GoogleNaturalLanguage")
    FString gcsContentUri = "";
};


class GoogleNaturalLanguageValueMapping
{
public:
    GoogleNaturalLanguageValueMapping();

    TMap<ENLDocumentType, FString> documentType;
    TMap<ENLLanguage, FString> languagCodes;
    TMap<ENLEncodingType, FString> encodingTypes;
    TMap<ENLEntityType, FString> entityTypes;
    TMap<ENLMentionType, FString> mentionTypes;
    TMap<ENLTag, FString> tagTypes;
    TMap<ENLAspect, FString> aspectTypes;
    TMap<ENLCase, FString> caseTypes;
    TMap<ENLForm, FString> formTypes;
    TMap<ENLGender, FString> genderTypes;
    TMap<ENLMood, FString> moodTypes;
    TMap<ENLNumber, FString> numberTypes;
    TMap<ENLPerson, FString> personTypes;
    TMap<ENLProper, FString> properTypes;
    TMap<ENLReciprocity, FString> reciprocityTypes;
    TMap<ENLTense, FString> tenseTypes;
    TMap<ENLVoice, FString> voiceTypes;
    TMap<ENLLabel, FString> labelTypes;
};

