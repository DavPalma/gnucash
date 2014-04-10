;; -*-scheme-*-
;;
;; This file was copied from the file txf.scm by  Richard -Gilligan- Uschold
;;
;; Originally, these were meant to hold the codes for the US tax TXF
;; format. I modified this heavily so that it might become useful for
;; the German Umsatzsteuer-Voranmeldung. 
;;
;; This file holds the explanations to the categories from txf-de_DE.scm.
;;

(define txf-help-strings
  '(
    (H001 . "Categories marked with a \"&lt;\" or a \"^\", require a Payer identification to be exported.  \"&lt;\" indicates that the name of this account is exported as this Payer ID.  Typically, this is a bank, stock, or mutual fund name.")
    (H002 . "Categories marked with a \"&lt;\" or a \"^\", require a Payer identification to be exported.  \"^\" indicates that the name of the PARENT of this account is exported as this Payer ID.  Typically, this is a bank, stock, or mutual fund name.")
    (H003 . "Categories marked with a \"#\" are not fully implemented yet!  Do not use these codes!")
    (N000 . "This is a dummy category and only shows up on the tax report, but is not exported.")


    (K41 . "Innergemeinschaftliche Lieferungen (� 4 Nr. 1 Buchst. b UStG) an Abnehmer mit USt-IdNr.  (Bemessungsgrundlage)")
    (K44 . "Innergemeinschaftliche Lieferungen neuer Fahrzeuge an Abnehmer ohne USt-IdNr (Bemessungsgrundlage)")
    (K49 . "Innergemeinschaftliche Lieferungen neuer Fahrzeuge au�erhalb eines Unternehmens (� 2a UStG)  (Bemessungsgrundlage)")
    (K43 . "Weitere steuerfreie Ums�tze mit Vorsteuerabzug (z.B. Ausfuhrlieferungen, Ums�tze nach � 4 Nr. 2 bis 7 UStG) (Bemessungsgrundlage)")
    (K48 . "Steuerfreie Ums�tze ohne Vorsteuerabzug: Ums�tze nach � 4 Nr. 8 bis 28 UStG (Bemessungsgrundlage)")

    (K51 . "Steuerpflichtige Ums�tze (Lieferungen und sonstige Leistungen einschl. unentgeltlicher Wertabgaben) zum Steuersatz von 16 v.H. (Bemessungsgrundlage)")
    (K86 . "Steuerpflichtige Ums�tze (Lieferungen und sonstige Leistungen einschl. unentgeltlicher Wertabgaben) zum Steuersatz von 7 v.H. (Bemessungsgrundlage)")
    (K35 . "Ums�tze, die anderen Steuers�tzen unterliegen (Bemessungsgrundlage)")
    (K36 . "Ums�tze, die anderen Steuers�tzen unterliegen (Steuer)")
    (K77 . "Ums�tze land- und forstwirtschaftlicher Betriebe nach � 24 UStG: Lieferungen in das �brige Gemeinschaftsgebiet an Abnehmer mit USt-IdNr. (Bemessungsgrundlage)")
    (K76 . "Ums�tze, f�r die eine Steuer nach � 24 UStG zu entrichten ist (S�gewerkserzeugnisse, Getr�nke und alkohol. Fl�ssigkeiten, z.B. Wein) (Bemessungsgrundlage)")
    (K80 . "Ums�tze, f�r die eine Steuer nach � 24 UStG zu entrichten ist (S�gewerkserzeugnisse, Getr�nke und alkohol. Fl�ssigkeiten, z.B. Wein) (Steuer)")

    (K91 . "Steuerfreie innergemeinschaftliche Erwerbe: Erwerbe nach � 4b UStG (Bemessungsgrundlage)")


    (K66 . "Vorsteuerbetr�ge aus Rechnungen von anderen Unternehmern (� 15 Abs. 1 Satz 1 Nr. 1 UStG), aus Leistungen im Sinne des � 13a Abs. 1 Nr. 6 UStG (� 15 Abs. 1 Satz 1 Nr. 5 UStG) und aus innergemeinschaftlichen Dreiecksgesch�ften (� 25b Abs. 5 UStG)")
    ))