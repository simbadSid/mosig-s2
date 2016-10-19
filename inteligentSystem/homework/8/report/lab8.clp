



; -------------------------------------------------------
; Used templates
; -------------------------------------------------------
	(deftemplate WordPair				; Structure for count Word Pair
		(slot CATEGORY	(type SYMBOL))
		(slot WORD1	(type SYMBOL))
		(slot WORD2	(type SYMBOL))
		(slot COUNT	(type INTEGER))		; Number of instances of word pair
	)


	(deftemplate Category				; A category of text (e.g. spam...)
		(slot NAME)	(type SYMBOL))		; Name for Category of Text
		(slot M		(type INTEGER))		; Size of Training Set for Category
	)

; -------------------------------------------------------
; Question a:
; -------------------------------------------------------
	; First we create all the pair of all the world
	(defrule MakeWorldPair
		?p <- (Paragraph (class ?cat) (?w1 $?wordList))
	=>
		(retract p)
		(bind ?i 1)
		(while (<= i (length ?wordList)))
			(assert (WordPair (CATEGORY ?cat) (WORD1 ?w1) (WORD2 (nth$ ?i wordList)) (COUNT 1)))
			(bind ?i (+ ?i 1))
		)
		
	)
	
	

; -------------------------------------------------------
; Question b:
; -------------------------------------------------------
	; Second we gather the pairs which are equivalent
	(defrule CountWordPair
		?wp1 <- (WordPair (CATEGORY ?c) (WORD1 ?w1) (WORD2 ?w2) (COUNT ?count1))
		?wp1 <- (WordPair (CATEGORY ?c) (WORD1 ?w1) (WORD2 ?w2) (COUNT ?count2))
		?cat <- (Category (NAME ?n) (M size))
	=>
		(retract ?wp1)
		(retract ?wp2)
		(assert (WordPair (CATEGORY ?c) (WORD1 ?w1) (WORD2 ?w2) (COUNT (+ ?count1 ?count2))))

		(retract ?cat)
		(assert (Category (NAME ?n) (M (+ ?size 1))))
	)
	
	

; -------------------------------------------------------
; Question c:
; -------------------------------------------------------

	(defrule CountAllWordPair
		?wp  <- (WordPair (CATEGORY ?c)    (WORD1 ?) (WORD2 ?) (COUNT c))
		?cat <- (WordPair (CATEGORY "All") (WORD1 ?) (WORD2 ?) (COUNT cAll))
	)
	
	
	
	
