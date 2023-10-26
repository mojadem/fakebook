# fakebook

Fakebook is a TikTok-inspired mobile application that uses OpenAI’s DALL-E 2 image generation API along with randomly generated keywords to generate endless unique, artificial content.

## Demo

![Demo](./demo.gif)

[Full Documentation Video](https://drive.google.com/file/d/13Z5fcmwQAz40yc3dbGC98_zp-XDESRPg/view?usp=sharing)

## Concept

As AI becomes more advanced, we see more and more industries being disrupted by its capabilities. Lately, we have seen the emergence of AI image generation, capable of creating compelling artwork via user specified keywords. There are also increasingly more examples of AI generated entertainment, such as the procedurally generated sitcom [Nothing, Forever](https://en.wikipedia.org/wiki/Nothing,_Forever), which was originally trained on the characters of Seinfeld.

The premise of this project was to combine artificial image generation with randomly generated keywords. This would essentially create an infinite loop of content generation. To do this, we combined a [random word API](https://random-word-api.herokuapp.com/home) with OpenAI's [DALL-E 2](https://openai.com/dall-e-2) API. So, to generate each image, we first pull a set of random keywords then feed this into the image generator to create a unique random image.

We then built a mobile app around this that mimicked many features of social media such as algorithm personalization and infinite scroll. These both often hold a negative connotation as they are credited for social media's addictive nature. To implement our rudimentary "social media algorithm", we simply track the time the user spends looking at an image. The longer they look at an image, the more likely it is that the image's keywords will be recycled in future image generations. Infinite scroll was implemented by multithreading the API calls so that images are loaded dynamically as the user scrolls.

## Tech

Building this project with openFrameworks, which is a rather antiquated but capable framework, presented quite a few challenges, the main one being API requests. Coming from Python, I had plenty of experience executing HTTP requests through the [requests](https://pypi.org/project/requests/) Python package, which is very easy to use and elegant. C++ has alternatives to this, such as [cpr](https://github.com/libcpr/cpr); however, I ran into issues trying to integrate this package, along with many other modern C++ libraries, into an openFrameworks project.

Because of this, I was forced to execute API requests directly through the [libcurl](https://curl.se/libcurl/) C library. This exposed me to many parts of HTTP requests that were previously hidden from me, and in the end I gained a much better understanding of the process than I had before. I also was forced to engineer my own multithreading solution to make these calls asynchronous; after looking into a variety of options, I decided to leverage [C++ futures](https://cplusplus.com/reference/future/future/) for this, essentially creating an array of futures that would load the next images in the background, allowing the UI to remain responsive and fluid.

Although this project was not completed using the most relevant tech stack, I learned a lot from the implementation, and vastly improved not only my C++ skills, but my knowledge of HTTP requests and multithreading as well.

## Reflection

Upon completion, I found this project to be very compelling. Scrolling was addictive, as you never knew what kind of image the algorithm would generate next. I do believe there is a future for applications like this; unfortunately, the costs of the image generation API prevented me from pursuing the idea any further. Maybe someday I'll revisit this idea given a more economic way to generate artificial content.

## Credits

This project was completed in collaboration with Trevor Barlock. The application was created using the open-source C++ creative coding framework [openFrameworks](https://openframeworks.cc/). The source code in this repository would be placed in the `src` folder within an openFrameworks project.
