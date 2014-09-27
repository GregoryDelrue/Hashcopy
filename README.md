Hashcopy
========

tiny tool to copy files based on their cryptographic hash value ~~(SHA3 512 bit)~~ (using any hashing algorithm that comes with Qt) eliminating doubles with different filenames but identical content

I recently was told that such a tool already exists and is called fdupes.
Since it uses only the MD5 hash algorithm instead of whatever the user chooses, lets call this "exploration of the parameter space": 
http://www.phdcomics.com/comics/archive.php?comicid=797


As of right now it compiles and not much more.
You can choose a directory and it will eliminate all  duplicate files inside by moving them into a /internalDoubles/ directory.
When you have a lot of files the UI will freeze up while the program keeps on working.
I will probably move the UI-code and the heavy-lifting-code to different threads in the future.
(eleminating dupes from a directory with 6000 pictures took around 3 minutes)


Next version is planned for: 
- whenever I feel like it.

It will include: 
- whatever feature comes to my mind.

(Im not exactly taking this serious, you might be better off to fork it and continue developing it yourself)

