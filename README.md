# fakebook

Fakebook is a TikTok-inspired mobile application that uses OpenAI’s DALL-E 2 image generation API along with randomly generated keywords to generate endless unique, artificial content.

## Demo

![Demo](./demo.gif)

[Full Documentation Video](https://drive.google.com/file/d/13Z5fcmwQAz40yc3dbGC98_zp-XDESRPg/view?usp=sharing)

## Concept

As AI becomes more advanced, we see more and more industries being disrupted by its capabilities. Lately, we have seen the emergence of AI image generation, capable of creating compelling artwork via user specified keywords. There are also increasingly more examples of AI generated entertainment, such as the procedurally generated sitcom [Nothing, Forever](https://en.wikipedia.org/wiki/Nothing,_Forever), which was originally trained on the characters of Seinfeld.

The premise of this project was to combine artificial image generation with randomly generated keywords. This would essentially create an infinite loop of content generation. To do this, we combined a [random word API](https://random-word-api.herokuapp.com/home) with OpenAI's [DALL-E 2](https://openai.com/dall-e-2) API. So, to generate each image, we first pull a set of random keywords then feed this into the image generator to create a unique random image.

We then built a mobile app around this that mimicked many features of social media such as algorithm personalization and infinite scroll. These both often hold a negative connotation as they are credited for social media's addictive nature. To implement our rudimentary "social media algorithm", we simply track the time the user spends looking at an image. The longer they look at an image, the more likely it is that the image's keywords will be recycled in future image generations. Infinite scroll was implemented by multithreading the API calls so that images are loaded dynamically as the user scrolls.

## Credits

This project was completed in collaboration with Trevor Barlock. The application was created using the open-source C++ creative coding framework [openFrameworks](https://openframeworks.cc/). The source code in this repository would be placed in the `src` folder within an openFrameworks project.
